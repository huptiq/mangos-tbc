#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Item.h"
#include "../../../../Globals/ObjectMgr.h"
#include <sstream>

void EnsureMilestoneEntryExists(Player *player)
{
    char query[256];
    sprintf(query, "SELECT guid FROM custom_milestones WHERE guid = %u", player->GetGUIDLow());

    std::unique_ptr<QueryResult> result(CharacterDatabase.Query(query));
    if (!result || result->GetRowCount() == 0)
    {
        sprintf(query, "INSERT INTO custom_milestones (guid, milestone_1, milestone_2, milestone_3) VALUES (%u, 0, 0, 0)", player->GetGUIDLow());
        CharacterDatabase.DirectExecute(query);
    }
}

void SetMilestone(Player *player, uint32 milestone)
{
    char query[256];
    sprintf(query, "SELECT guid FROM custom_milestones WHERE guid = %u", player->GetGUIDLow());

    std::unique_ptr<QueryResult> result(CharacterDatabase.Query(query));
    if (result && result->GetRowCount() > 0)
    {
        CharacterDatabase.PExecute("UPDATE custom_milestones SET milestone_%u = 1 WHERE guid = %u", milestone, player->GetGUIDLow());
    }
}

bool GossipHello_milestone_npc(Player *player, Creature *creature)
{
    if (player->GetSession()->GetOS() != 0)
    {
        EnsureMilestoneEntryExists(player);
        std::unique_ptr<QueryResult> result = CharacterDatabase.PQuery("SELECT milestone_1, milestone_2, milestone_3 FROM custom_milestones WHERE guid = %u", player->GetGUIDLow());

        player->ADD_GOSSIP_ITEM(7, "", GOSSIP_SENDER_MAIN, 999);
        if (player->GetLevel() > 1)
        {
            if (player->GetLevel() >= 20 && (*result)[0].GetUInt32() == 0)
            {
                player->ADD_GOSSIP_ITEM(4, " Milestone Pet I", GOSSIP_SENDER_MAIN, 1);
                player->ADD_GOSSIP_ITEM(4, " Milestone Pet II", GOSSIP_SENDER_MAIN, 4);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
            }
            if (player->GetLevel() >= 40 && (*result)[1].GetUInt32() == 0)
            {
                player->ADD_GOSSIP_ITEM(4, " Milestone Toy", GOSSIP_SENDER_MAIN, 2);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
            }
            if (player->GetLevel() >= 70 && (*result)[2].GetUInt32() == 0)
            {
                player->ADD_GOSSIP_ITEM(4, " Milestone Mount", GOSSIP_SENDER_MAIN, 3);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
            }
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
        }
    }

    return true;
}

bool GossipSelect_milestone_npc(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    switch (action)
    {
    case 999:
        break;
    case 998:
        player->GetPlayerMenu()->ClearMenus();
        GossipHello_milestone_npc(player, creature);
        break;
    case 1: // Milestone Pet I
        player->GetPlayerMenu()->ClearMenus();
        player->ADD_GOSSIP_ITEM(7, " -- Choose your pet! --", GOSSIP_SENDER_MAIN, 999);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Hippogryph Hatchling", GOSSIP_SENDER_MAIN, 20, "Hippogryph Hatchling", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Banana Charm", GOSSIP_SENDER_MAIN, 21, "Banana Charm", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Rocket Chicken", GOSSIP_SENDER_MAIN, 22, "Rocket Chicken", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Blue Murloc Egg", GOSSIP_SENDER_MAIN, 23, "Blue Murloc Egg", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Orange Murloc Egg", GOSSIP_SENDER_MAIN, 24, "Orange Murloc Egg", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Pink Murloc Egg", GOSSIP_SENDER_MAIN, 25, "Pink Murloc Egg", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " White Murloc Egg", GOSSIP_SENDER_MAIN, 26, "White Murloc Egg", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Zergling Leash", GOSSIP_SENDER_MAIN, 27, "Zergling Leash", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Diablo Stone", GOSSIP_SENDER_MAIN, 28, "Diablo Stone", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Panda Collar", GOSSIP_SENDER_MAIN, 29, "Panda Collar", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Golden Pig Coin", GOSSIP_SENDER_MAIN, 30, "Golden Pig Coin", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Silver Pig Coin", GOSSIP_SENDER_MAIN, 31, "Silver Pig Coin", 0, false);
        player->ADD_GOSSIP_ITEM(4, " Back", GOSSIP_SENDER_MAIN, 998);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
        break;
    case 4: // Milestone Pet II
        player->GetPlayerMenu()->ClearMenus();
        player->ADD_GOSSIP_ITEM(7, " -- Choose your pet! --", GOSSIP_SENDER_MAIN, 999);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Dragon Kite", GOSSIP_SENDER_MAIN, 32, "Dragon Kite", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Netherwhelp's Collar", GOSSIP_SENDER_MAIN, 33, "Netherwhelp's Collar", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Tyrael's Hilt", GOSSIP_SENDER_MAIN, 34, "Tyrael's Hilt", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Soul-Trader Beacon", GOSSIP_SENDER_MAIN, 35, "Soul-Trader Beacon", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Competitor's Souvenir", GOSSIP_SENDER_MAIN, 36, "Competitor's Souvenir", 0, false);
        player->ADD_GOSSIP_ITEM(4, " Back", GOSSIP_SENDER_MAIN, 998);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
        break;
    case 2: // Milestone Toy
        player->GetPlayerMenu()->ClearMenus();
        player->ADD_GOSSIP_ITEM(7, " -- Choose your toy! --", GOSSIP_SENDER_MAIN, 999);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Imp in a Ball", GOSSIP_SENDER_MAIN, 40, "Imp in a Ball", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Picnic Basket", GOSSIP_SENDER_MAIN, 41, "Picnic Basket", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Fishing Chair", GOSSIP_SENDER_MAIN, 42, "Fishing Chair", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Goblin Gumbo Kettle", GOSSIP_SENDER_MAIN, 43, "Goblin Gumbo Kettle", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Paper Flying Machine Kite", GOSSIP_SENDER_MAIN, 44, "Paper Flying Machine Kite", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Goblin Weather Machine - Prototype 01-B", GOSSIP_SENDER_MAIN, 45, "Goblin Weather Machine - Prototype 01-B", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Papa Hummel's Old-Fashioned Pet Biscuit", GOSSIP_SENDER_MAIN, 46, "Papa Hummel's Old-Fashioned Pet Biscuit", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " D.I.S.C.O.", GOSSIP_SENDER_MAIN, 47, "D.I.S.C.O.", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Path of Illidan", GOSSIP_SENDER_MAIN, 48, "Path of Illidan", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " The Flag of Ownership", GOSSIP_SENDER_MAIN, 49, "The Flag of Ownership", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Party G.R.E.N.A.D.E.", GOSSIP_SENDER_MAIN, 50, "Party G.R.E.N.A.D.E.", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Murloc Costume", GOSSIP_SENDER_MAIN, 51, "Murloc Costume", 0, false);
        player->ADD_GOSSIP_ITEM(4, " Back", GOSSIP_SENDER_MAIN, 998);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
        break;
    case 3: // Milestone Mount
        player->GetPlayerMenu()->ClearMenus();
        player->ADD_GOSSIP_ITEM(7, " -- Choose your mount! --", GOSSIP_SENDER_MAIN, 999);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Riding Turtle", GOSSIP_SENDER_MAIN, 60, "Riding Turtle", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Reins of the Spectral Tiger", GOSSIP_SENDER_MAIN, 67, "Reins of the Spectral Tiger", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Reins of the Swift Spectral Tiger", GOSSIP_SENDER_MAIN, 61, "Reins of the Swift Spectral Tiger", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " X-51 Nether-Rocket", GOSSIP_SENDER_MAIN, 65, "X-51 Nether-Rocket", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " X-51 Nether-Rocket X-TREME", GOSSIP_SENDER_MAIN, 62, "X-51 Nether-Rocket X-TREME", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Big Battle Bear", GOSSIP_SENDER_MAIN, 63, "Big Battle Bear", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Swift Zhevra", GOSSIP_SENDER_MAIN, 64, "Swift Zhevra", 0, false);
        player->ADD_GOSSIP_ITEM_EXTENDED(4, " Black Qiraji Resonating Crystal", GOSSIP_SENDER_MAIN, 66, "Black Qiraji Resonating Crystal", 0, false);
        player->ADD_GOSSIP_ITEM(4, " Back", GOSSIP_SENDER_MAIN, 998);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
        break;
    // Milestone I Loot
    case 20:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(23713, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 21:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(32588, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 22:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(34492, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 23:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(20371, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 24:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(20651, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 25:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(22114, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 26:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(22780, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 27:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(13582, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 28:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(13584, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 29:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(13583, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 30:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(34518, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 31:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(34519, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 32:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(34493, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 33:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(25535, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 34:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(39656, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 35:
        SetMilestone(player, 1);
        player->StoreNewItemInInventorySlot(37298, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    // Milestone II Loot
    case 40:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(32542, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 41:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(32566, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 42:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(33223, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 43:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(33219, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 44:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(34499, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 45:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(35227, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 46:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(35223, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 47:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(38301, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 48:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(38233, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 49:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(38578, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 50:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(38577, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 51:
        SetMilestone(player, 2);
        player->StoreNewItemInInventorySlot(33079, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    // Milestone III Loot
    case 60:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(23720, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 61:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(33225, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 62:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(35226, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 63:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(38576, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 64:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(37719, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 65:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(35225, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 66:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(21176, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 67:
        SetMilestone(player, 3);
        player->StoreNewItemInInventorySlot(33224, 1);
        player->CLOSE_GOSSIP_MENU();
        break;
    }
    return true;
}

void AddSC_milestone_npc()
{
    auto newscript = new Script;
    newscript->Name = "custom_milestone_npc";
    newscript->pGossipHello = &GossipHello_milestone_npc;
    newscript->pGossipSelect = &GossipSelect_milestone_npc;
    newscript->RegisterSelf(false);
}
