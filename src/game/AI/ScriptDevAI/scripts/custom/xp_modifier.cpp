#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include <sstream>

float xp_modifier;

void LoadXPModifier(Player *player)
{
    std::stringstream playerUIDStr;
    playerUIDStr << player->GetGUIDLow();

    std::string query = "SELECT modifier FROM custom_xp WHERE player_uid = " + playerUIDStr.str();
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query(query.c_str()));

    if (result && result->GetRowCount() > 0)
    {
        xp_modifier = result->Fetch()[0].GetFloat();
    }
    else
    {
        xp_modifier = 1.0f;
    }
}

void UpdateCustomXP(Player *player, uint32 player_uid, float xp_modifier)
{
    char query[256];
    sprintf(query, "SELECT modifier FROM custom_xp WHERE player_uid = %u", player_uid);

    std::unique_ptr<QueryResult> result(CharacterDatabase.Query(query));
    if (result && result->GetRowCount() > 0)
    {
        sprintf(query, "UPDATE custom_xp SET modifier = %.2f WHERE player_uid = %u", xp_modifier, player_uid);
        CharacterDatabase.DirectExecute(query);
    }
    else
    {
        sprintf(query, "INSERT INTO custom_xp (player_uid, modifier) VALUES (%u, %.2f)", player_uid, xp_modifier);
        CharacterDatabase.DirectExecute(query);
    }
}

bool GossipHello_custom_xp(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM(10, "Choose your experience modifier", GOSSIP_SENDER_MAIN, 1);

    std::stringstream playerUIDStr;
    playerUIDStr << player->GetGUIDLow();

    std::string query = "SELECT modifier FROM custom_xp WHERE player_uid = " + playerUIDStr.str();
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query(query.c_str()));

    if (result && result->GetRowCount() > 0)
    {
        float modifier = result->Fetch()[0].GetFloat();
        std::string custom_npc_string = std::to_string(modifier);
        player->ADD_GOSSIP_ITEM(10, "Current Modifier: x" + custom_npc_string.substr(0, 4), GOSSIP_SENDER_MAIN, 1);
    }
    player->ADD_GOSSIP_ITEM(3, "x0.0 - off", GOSSIP_SENDER_MAIN, 2);
    player->ADD_GOSSIP_ITEM(3, "x1.0 - default", GOSSIP_SENDER_MAIN, 3);
    player->ADD_GOSSIP_ITEM(3, "x1.1 - 110%", GOSSIP_SENDER_MAIN, 4);
    player->ADD_GOSSIP_ITEM(3, "x1.2 - 120%", GOSSIP_SENDER_MAIN, 5);
    player->ADD_GOSSIP_ITEM(3, "x1.3 - 130%", GOSSIP_SENDER_MAIN, 6);
    player->ADD_GOSSIP_ITEM(3, "x1.4 - 140%", GOSSIP_SENDER_MAIN, 7);
    player->ADD_GOSSIP_ITEM(3, "x1.5 - 150%", GOSSIP_SENDER_MAIN, 8);
    player->ADD_GOSSIP_ITEM(3, "x2.0 - 200%", GOSSIP_SENDER_MAIN, 9);
    player->ADD_GOSSIP_ITEM(3, "x5.0 - 500%", GOSSIP_SENDER_MAIN, 10);
    player->ADD_GOSSIP_ITEM(3, "x10.0 - 1000%", GOSSIP_SENDER_MAIN, 11);
    player->ADD_GOSSIP_ITEM(3, "x20.0 - 2000%", GOSSIP_SENDER_MAIN, 12);
    player->ADD_GOSSIP_ITEM(3, "x50.0 - 5000%", GOSSIP_SENDER_MAIN, 13);
    if (player->GetLevel() == 1)
    {
        player->ADD_GOSSIP_ITEM(3, "skip to 60", GOSSIP_SENDER_MAIN, 14);
    }
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_custom_xp(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    class Item
    {
    public:
        Item(uint32 itemId) : m_itemId(itemId) {}

    private:
        uint32 m_itemId;
    };
    switch (action)
    {
    case 1:
        break;
    case 2:
        UpdateCustomXP(player, player->GetGUIDLow(), 0.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 3:
        UpdateCustomXP(player, player->GetGUIDLow(), 1.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 4:
        UpdateCustomXP(player, player->GetGUIDLow(), 1.1f);
        player->GetSession()->LogoutPlayer();
        break;
    case 5:
        UpdateCustomXP(player, player->GetGUIDLow(), 1.2f);
        player->GetSession()->LogoutPlayer();
        break;
    case 6:
        UpdateCustomXP(player, player->GetGUIDLow(), 1.3f);
        player->GetSession()->LogoutPlayer();
        break;
    case 7:
        UpdateCustomXP(player, player->GetGUIDLow(), 1.4f);
        player->GetSession()->LogoutPlayer();
        break;
    case 8:
        UpdateCustomXP(player, player->GetGUIDLow(), 1.5f);
        player->GetSession()->LogoutPlayer();
        break;
    case 9:
        UpdateCustomXP(player, player->GetGUIDLow(), 2.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 10:
        UpdateCustomXP(player, player->GetGUIDLow(), 5.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 11:
        UpdateCustomXP(player, player->GetGUIDLow(), 10.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 12:
        UpdateCustomXP(player, player->GetGUIDLow(), 20.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 13:
        UpdateCustomXP(player, player->GetGUIDLow(), 50.0f);
        player->GetSession()->LogoutPlayer();
        break;
    case 14:
        player->SetLevel(60);
        if (player->getClass() == 1) // Warrior
        {
            player->learnSpell(33388, 0, 0);                         // riding
            player->learnSpell(750, 0, 0);                           // plate armor
            player->learnSpell(5011, 0, 0);                          // crossbows
            player->learnSpell(266, 0, 0);                           // guns
            player->learnSpell(264, 0, 0);                           // bows
            player->learnSpell(227, 0, 0);                           // staves
            player->learnSpell(197, 0, 0);                           // two-handed axes
            player->learnSpell(202, 0, 0);                           // two-handed swords
            player->learnSpell(199, 0, 0);                           // two-handed maces
            player->learnSpell(196, 0, 0);                           // one-handed axes
            player->learnSpell(198, 0, 0);                           // one-handed maces
            player->learnSpell(201, 0, 0);                           // one-handed swords
            player->learnSpell(1180, 0, 0);                          // daggers
            player->learnSpell(15590, 0, 0);                         // fist-weapon
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(226, 300, player->GetSkillMax(226), 0); // crossbow
            player->SetSkill(46, 300, player->GetSkillMax(46), 0);   // gun
            player->SetSkill(45, 300, player->GetSkillMax(45), 0);   // bow
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(172, 300, player->GetSkillMax(172), 0); // two-handed axes
            player->SetSkill(55, 300, player->GetSkillMax(5), 0);    // two-handed sword
            player->SetSkill(160, 300, player->GetSkillMax(160), 0); // two-handed maces
            player->SetSkill(44, 300, player->GetSkillMax(44), 0);   // one-handed axes
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->SetSkill(43, 300, player->GetSkillMax(43), 0);   // one-handed swords
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(473, 300, player->GetSkillMax(473), 0); // fist-weapon
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 13404, 1);
            player->EquipNewItem(2, 15411, 1);
            player->EquipNewItem(3, 12927, 1);
            player->EquipNewItem(5, 11726, 1);
            player->EquipNewItem(6, 13142, 1);
            player->EquipNewItem(7, 14554, 1);
            player->EquipNewItem(8, 14616, 1);
            player->EquipNewItem(9, 12936, 1);
            player->EquipNewItem(10, 14551, 1);
            player->EquipNewItem(11, 17713, 1);
            player->EquipNewItem(12, 13098, 1);
            player->EquipNewItem(13, 13965, 1);
            player->EquipNewItem(14, 11815, 1);
            player->EquipNewItem(15, 13340, 1);
            player->EquipNewItem(16, 12940, 1);
            player->EquipNewItem(17, 12939, 1);
            player->EquipNewItem(18, 12651, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            player->StoreNewItemInInventorySlot(15806, 1);
            player->StoreNewItemInInventorySlot(12602, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 2) // Paladin
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(750, 0, 0);
            player->learnSpell(197, 0, 0);
            player->learnSpell(202, 0, 0);
            player->learnSpell(199, 0, 0);
            player->learnSpell(196, 0, 0);
            player->learnSpell(198, 0, 0);
            player->learnSpell(201, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(172, 300, player->GetSkillMax(172), 0); // two-handed axes
            player->SetSkill(55, 300, player->GetSkillMax(5), 0);    // two-handed sword
            player->SetSkill(160, 300, player->GetSkillMax(160), 0); // two-handed maces
            player->SetSkill(44, 300, player->GetSkillMax(44), 0);   // one-handed axes
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->SetSkill(43, 300, player->GetSkillMax(43), 0);   // one-handed swords
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 13404, 1);
            player->EquipNewItem(2, 15411, 1);
            player->EquipNewItem(3, 12927, 1);
            player->EquipNewItem(5, 11726, 1);
            player->EquipNewItem(6, 13959, 1);
            player->EquipNewItem(7, 14554, 1);
            player->EquipNewItem(8, 14616, 1);
            player->EquipNewItem(9, 12936, 1);
            player->EquipNewItem(10, 13957, 1);
            player->EquipNewItem(11, 13098, 1);
            player->EquipNewItem(12, 12548, 1);
            player->EquipNewItem(13, 13965, 1);
            player->EquipNewItem(14, 11815, 1);
            player->EquipNewItem(15, 13340, 1);
            player->EquipNewItem(16, 12784, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            player->StoreNewItemInInventorySlot(11784, 1);
            player->StoreNewItemInInventorySlot(12602, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 3) // Hunter
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(15590, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(8737, 0, 0); // mail armor
            player->learnSpell(5011, 0, 0);
            player->learnSpell(266, 0, 0);
            player->learnSpell(264, 0, 0);
            player->learnSpell(227, 0, 0);
            player->learnSpell(197, 0, 0);
            player->learnSpell(202, 0, 0);
            player->learnSpell(196, 0, 0);
            player->learnSpell(201, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(226, 300, player->GetSkillMax(226), 0); // crossbow
            player->SetSkill(46, 300, player->GetSkillMax(46), 0);   // gun
            player->SetSkill(45, 300, player->GetSkillMax(45), 0);   // bow
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(172, 300, player->GetSkillMax(172), 0); // two-handed axes
            player->SetSkill(55, 300, player->GetSkillMax(5), 0);    // two-handed sword
            player->SetSkill(44, 300, player->GetSkillMax(44), 0);   // one-handed axes
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->SetSkill(43, 300, player->GetSkillMax(43), 0);   // one-handed swords
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(473, 300, player->GetSkillMax(473), 0); // fist-weapon
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 13404, 1);
            player->EquipNewItem(2, 15411, 1);
            player->EquipNewItem(3, 12927, 1);
            player->EquipNewItem(5, 11726, 1);
            player->EquipNewItem(6, 14502, 1);
            player->EquipNewItem(7, 15062, 1);
            player->EquipNewItem(8, 13967, 1);
            player->EquipNewItem(9, 13211, 1);
            player->EquipNewItem(10, 15063, 1);
            player->EquipNewItem(11, 13098, 1);
            player->EquipNewItem(12, 17713, 1);
            player->EquipNewItem(13, 13965, 1);
            player->EquipNewItem(14, 11815, 1);
            player->EquipNewItem(15, 13340, 1);
            player->EquipNewItem(16, 12940, 1);
            player->EquipNewItem(17, 12939, 1);
            player->EquipNewItem(18, 18738, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 2662, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 4) // Rogue
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(15590, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(5011, 0, 0);
            player->learnSpell(266, 0, 0);
            player->learnSpell(264, 0, 0);
            player->learnSpell(198, 0, 0);
            player->learnSpell(201, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(226, 300, player->GetSkillMax(226), 0); // crossbow
            player->SetSkill(46, 300, player->GetSkillMax(46), 0);   // gun
            player->SetSkill(45, 300, player->GetSkillMax(45), 0);   // bow
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->SetSkill(43, 300, player->GetSkillMax(43), 0);   // one-handed swords
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(473, 300, player->GetSkillMax(473), 0); // fist-weapon
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 13404, 1);
            player->EquipNewItem(2, 15411, 1);
            player->EquipNewItem(3, 12937, 1);
            player->EquipNewItem(5, 14637, 1);
            player->EquipNewItem(6, 13252, 1);
            player->EquipNewItem(7, 15062, 1);
            player->EquipNewItem(8, 12553, 1);
            player->EquipNewItem(9, 13120, 1);
            player->EquipNewItem(10, 15063, 1);
            player->EquipNewItem(11, 13098, 1);
            player->EquipNewItem(12, 17713, 1);
            player->EquipNewItem(13, 13965, 1);
            player->EquipNewItem(14, 11815, 1);
            player->EquipNewItem(15, 13340, 1);
            player->EquipNewItem(16, 12940, 1);
            player->EquipNewItem(17, 12939, 1);
            player->EquipNewItem(18, 12651, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 5) // Priest
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(5009, 0, 0); // wands
            player->learnSpell(227, 0, 0);
            player->learnSpell(198, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(228, 300, player->GetSkillMax(228), 0); // wands
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 10504, 1);
            player->EquipNewItem(2, 18691, 1);
            player->EquipNewItem(3, 14112, 1);
            player->EquipNewItem(5, 14136, 1);
            player->EquipNewItem(6, 1162, 1);
            player->EquipNewItem(7, 13170, 1);
            player->EquipNewItem(8, 18735, 1);
            player->EquipNewItem(9, 11766, 1);
            player->EquipNewItem(10, 13253, 1);
            player->EquipNewItem(11, 12545, 1);
            player->EquipNewItem(12, 13001, 1);
            player->EquipNewItem(13, 12930, 1);
            player->EquipNewItem(14, 11819, 1);
            player->EquipNewItem(15, 13386, 1);
            player->EquipNewItem(16, 13349, 1);
            player->EquipNewItem(17, 10796, 1);
            player->EquipNewItem(18, 13396, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 7) // Shaman Ele
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(15590, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(8737, 0, 0);
            player->learnSpell(227, 0, 0);
            player->learnSpell(197, 0, 0);
            player->learnSpell(199, 0, 0);
            player->learnSpell(196, 0, 0);
            player->learnSpell(198, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(172, 300, player->GetSkillMax(172), 0); // two-handed axes
            player->SetSkill(160, 300, player->GetSkillMax(160), 0); // two-handed maces
            player->SetSkill(44, 300, player->GetSkillMax(44), 0);   // one-handed axes
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(473, 300, player->GetSkillMax(473), 0); // fist-weapon
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 10504, 1);
            player->EquipNewItem(2, 12103, 1);
            player->EquipNewItem(3, 13013, 1);
            player->EquipNewItem(5, 11924, 1);
            player->EquipNewItem(6, 11662, 1);
            player->EquipNewItem(7, 13170, 1);
            player->EquipNewItem(8, 13954, 1);
            player->EquipNewItem(9, 11765, 1);
            player->EquipNewItem(10, 13253, 1);
            player->EquipNewItem(11, 12545, 1);
            player->EquipNewItem(12, 13001, 1);
            player->EquipNewItem(13, 12930, 1);
            player->EquipNewItem(14, 13968, 1);
            player->EquipNewItem(15, 15421, 1);
            player->EquipNewItem(16, 13964, 1);
            player->EquipNewItem(17, 11904, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            player->StoreNewItemInInventorySlot(5175, 1);
            player->StoreNewItemInInventorySlot(5176, 1);
            player->StoreNewItemInInventorySlot(5177, 1);
            player->StoreNewItemInInventorySlot(5178, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 8) // Mage
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(5009, 0, 0);
            player->learnSpell(227, 0, 0);
            player->learnSpell(201, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(43, 300, player->GetSkillMax(43), 0);   // one-handed swords
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(228, 300, player->GetSkillMax(228), 0); // wands
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 10504, 1);
            player->EquipNewItem(2, 12103, 1);
            player->EquipNewItem(3, 11782, 1);
            player->EquipNewItem(5, 14152, 1);
            player->EquipNewItem(6, 11662, 1);
            player->EquipNewItem(7, 13170, 1);
            player->EquipNewItem(8, 11822, 1);
            player->EquipNewItem(9, 11766, 1);
            player->EquipNewItem(10, 13253, 1);
            player->EquipNewItem(11, 942, 1);
            player->EquipNewItem(12, 942, 1);
            player->EquipNewItem(13, 12930, 1);
            player->EquipNewItem(14, 13968, 1);
            player->EquipNewItem(15, 13386, 1);
            player->EquipNewItem(16, 13964, 1);
            player->EquipNewItem(17, 10796, 1);
            player->EquipNewItem(18, 13938, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 9) // Warlock
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(5009, 0, 0);
            player->learnSpell(227, 0, 0);
            player->learnSpell(201, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(43, 300, player->GetSkillMax(43), 0);   // one-handed swords
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(228, 300, player->GetSkillMax(228), 0); // wands
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 10504, 1);
            player->EquipNewItem(2, 18691, 1);
            player->EquipNewItem(3, 14112, 1);
            player->EquipNewItem(5, 14153, 1);
            player->EquipNewItem(6, 11662, 1);
            player->EquipNewItem(7, 13170, 1);
            player->EquipNewItem(8, 18735, 1);
            player->EquipNewItem(9, 11766, 1);
            player->EquipNewItem(10, 13253, 1);
            player->EquipNewItem(11, 12545, 1);
            player->EquipNewItem(12, 13001, 1);
            player->EquipNewItem(13, 12930, 1);
            player->EquipNewItem(14, 13968, 1);
            player->EquipNewItem(15, 13386, 1);
            player->EquipNewItem(16, 13964, 1);
            player->EquipNewItem(17, 10796, 1);
            player->EquipNewItem(18, 13396, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            player->StoreNewItemInInventorySlot(6265, 4);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        else if (player->getClass() == 11) // Druid Feral
        {
            player->learnSpell(33388, 0, 0);
            player->learnSpell(1180, 0, 0);
            player->learnSpell(227, 0, 0);
            player->learnSpell(199, 0, 0);
            player->learnSpell(198, 0, 0);
            player->learnSpell(15590, 0, 0);
            player->SetSkill(95, 300, player->GetSkillMax(95), 0);   // defense
            player->SetSkill(136, 300, player->GetSkillMax(136), 0); // staves
            player->SetSkill(160, 300, player->GetSkillMax(160), 0); // two-handed maces
            player->SetSkill(54, 300, player->GetSkillMax(54), 0);   // one-handed maces
            player->SetSkill(173, 300, player->GetSkillMax(173), 0); // daggers
            player->SetSkill(473, 300, player->GetSkillMax(473), 0); // fist-weapon
            player->learnQuestRewardedSpells();
            player->EquipNewItem(1, 8345, 1);
            player->EquipNewItem(2, 15411, 1);
            player->EquipNewItem(3, 12927, 1);
            player->EquipNewItem(5, 13340, 1);
            player->EquipNewItem(6, 13252, 1);
            player->EquipNewItem(7, 15062, 1);
            player->EquipNewItem(8, 12553, 1);
            player->EquipNewItem(9, 16710, 1);
            player->EquipNewItem(10, 15063, 1);
            player->EquipNewItem(11, 17713, 1);
            player->EquipNewItem(12, 2246, 1);
            player->EquipNewItem(13, 13965, 1);
            player->EquipNewItem(14, 11815, 1);
            player->EquipNewItem(15, 13340, 1);
            player->EquipNewItem(16, 11921, 1);
            player->EquipNewItem(20, 3762, 1);
            player->EquipNewItem(21, 3762, 1);
            player->EquipNewItem(22, 3762, 1);
            player->EquipNewItem(23, 3762, 1);
            if (player->GetTeam() == ALLIANCE)
            {
                player->StoreNewItemInInventorySlot(2411, 1);
            }
            else if (player->GetTeam() == HORDE)
            {
                player->StoreNewItemInInventorySlot(5668, 1);
            }
        }
        // else if (player->getClass() == 6)     //Deathknight
        break;
    }
    return true;
}

void AddSC_custom_xp()
{
    auto newscript = new Script;
    newscript->Name = "custom_xp";
    newscript->pGossipHello = &GossipHello_custom_xp;
    newscript->pGossipSelect = &GossipSelect_custom_xp;
    newscript->RegisterSelf(false);
}
