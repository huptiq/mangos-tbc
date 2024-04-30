#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Item.h"
#include "../../../../Globals/ObjectMgr.h"
#include <sstream>

float xp_modifier;
const std::vector<uint32> Level1StartingGear = {25, 35, 36, 37, 39, 40, 43, 44, 47, 48, 51, 52, 55, 56, 57, 59, 117, 120, 121, 129, 139, 140, 147, 153, 159, 1395, 1396, 2070, 2092, 2101, 2102, 2361, 2362, 2504, 2508, 2512, 2516, 3661, 4536, 4540, 4604, 6098, 6116, 6118, 6119, 6121, 6122, 6123, 6124, 6126, 6127, 6129, 6135, 6137, 6138, 6139, 6140, 6144, 12282, 20857, 20891, 20892, 20893, 20894, 20895, 20896, 20898, 20899, 20900, 20980, 20981, 20982, 20983, 23322, 23344, 23346, 23347, 23348, 23474, 23475, 23477, 23478, 23479, 24145, 24146, 25861, 28979};

void LearnAvailableClassSpells(Player *player, bool includeQuestRewards)
{
    ChrClassesEntry const *clsEntry = sChrClassesStore.LookupEntry(player->getClass());
    if (!clsEntry)
        return;

    uint32 family = clsEntry->spellfamily;

    // special cases which aren't sourced from trainers and normally require quests to obtain - added here for convenience
    ObjectMgr::QuestMap const &qTemplates = sObjectMgr.GetQuestTemplates();
    for (const auto &qTemplate : qTemplates)
    {
        Quest const *quest = qTemplate.second;
        if (!quest)
            continue;

        // only class quests player could do
        if (quest->GetRequiredClasses() == 0 || !player->SatisfyQuestClass(quest, false) || !player->SatisfyQuestRace(quest, false) || !player->SatisfyQuestLevel(quest, false))
            continue;

        // custom filter for scripting purposes
        if (!includeQuestRewards && quest->GetMinLevel() >= 60)
            continue;

        player->learnQuestRewardedSpells(quest);
    }

    // learn trainer spells
    for (uint32 id = 0; id < sCreatureStorage.GetMaxEntry(); ++id)
    {
        CreatureInfo const *co = sCreatureStorage.LookupEntry<CreatureInfo>(id);
        if (!co)
            continue;

        if (co->TrainerType != TRAINER_TYPE_CLASS)
            continue;

        if (co->TrainerType == TRAINER_TYPE_CLASS && co->TrainerClass != player->getClass())
            continue;

        uint32 trainerId = co->TrainerTemplateId;
        if (!trainerId)
            trainerId = co->Entry;

        TrainerSpellData const *trainer_spells = sObjectMgr.GetNpcTrainerTemplateSpells(trainerId);
        if (!trainer_spells)
            trainer_spells = sObjectMgr.GetNpcTrainerSpells(trainerId);

        if (!trainer_spells)
            continue;

        for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
        {
            TrainerSpell const *tSpell = &itr->second;

            if (!tSpell)
                continue;

            uint32 reqLevel = 0;

            // skip wrong class/race skills
            if (!player->IsSpellFitByClassAndRace(tSpell->learnedSpell, &reqLevel))
                continue;

            if (tSpell->conditionId && !sObjectMgr.IsConditionSatisfied(tSpell->conditionId, player, player->GetMap(), player, CONDITION_FROM_TRAINER))
                continue;

            // skip spells with first rank learned as talent (and all talents then also)
            uint32 first_rank = sSpellMgr.GetFirstSpellInChain(tSpell->learnedSpell);
            reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);
            bool isValidTalent = GetTalentSpellCost(first_rank) && player->HasSpell(first_rank) && reqLevel <= player->GetLevel();

            TrainerSpellState state = player->GetTrainerSpellState(tSpell, reqLevel);
            if (state != TRAINER_SPELL_GREEN && !isValidTalent)
                continue;

            SpellEntry const *proto = sSpellTemplate.LookupEntry<SpellEntry>(tSpell->learnedSpell);
            if (!proto)
                continue;

            // fix activate state for non-stackable low rank (and find next spell for !active case)
            if (uint32 nextId = sSpellMgr.GetSpellBookSuccessorSpellId(proto->Id))
            {
                if (player->HasSpell(nextId))
                {
                    // high rank already known so this must !active
                    continue;
                }
            }

            // skip other spell families (minus a few exceptions)
            if (proto->SpellFamilyName != family)
            {
                SkillLineAbilityMapBounds bounds = sSpellMgr.GetSkillLineAbilityMapBoundsBySpellId(tSpell->learnedSpell);
                if (bounds.first == bounds.second)
                    continue;

                SkillLineAbilityEntry const *skillInfo = bounds.first->second;
                if (!skillInfo)
                    continue;

                switch (skillInfo->skillId)
                {
                case SKILL_SUBTLETY:
                case SKILL_BEAST_MASTERY:
                case SKILL_SURVIVAL:
                case SKILL_DEFENSE:
                case SKILL_DUAL_WIELD:
                case SKILL_FERAL_COMBAT:
                case SKILL_PROTECTION:
                case SKILL_PLATE_MAIL:
                case SKILL_DEMONOLOGY:
                case SKILL_ENHANCEMENT:
                case SKILL_MAIL:
                case SKILL_HOLY2:
                case SKILL_LOCKPICKING:
                    break;

                default:
                    continue;
                }
            }

            // skip wrong class/race skills
            if (!player->IsSpellFitByClassAndRace(tSpell->learnedSpell))
                continue;

            // skip broken spells
            if (!SpellMgr::IsSpellValid(proto, player, false))
                continue;

            if (tSpell->learnedSpell)
            {
                bool learned = false;
                for (int j = 0; j < 3; ++j)
                {
                    if (proto->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
                    {
                        uint32 learnedSpell = proto->EffectTriggerSpell[j];
                        player->learnSpell(learnedSpell, false);
                        learned = true;
                    }
                }

                if (!learned)
                {
                    player->learnSpell(tSpell->learnedSpell, false);
                }
            }
            else
            {
                player->CastSpell(player, tSpell->spell, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }

    player->PlaySpellVisual(249);
}

void SetMaxSkill(Player *player, uint16 id)
{
    SkillLineEntry const *sl = sSkillLineStore.LookupEntry(id);
    if (sl)
    {
        uint32 maxValue = 300;
        // do not let skill go beyond limit even if maxlevel > blizzlike
#if EXPANSION > 0
        if (player->GetLevel() > 60)
        {
#if EXPANSION == 1
            maxValue = (player->GetLevel() + 5) * 5; // tbc (70 + 5)*5 = 375
#else
            maxValue = (player->GetLevel() + 10) * 5; // wotlk (80 + 10)*5 = 450
#endif
        }
#endif

        uint32 maxPure = player->GetSkillMaxPure(id);
        if (maxPure > maxValue)
            maxValue = maxPure;

        if (player->GetSkillValue(id) < maxValue)
            player->SetSkill(id, maxValue, maxValue);
    }
}

void LearnWeaponSkills(Player *player)
{
    // Learn weapon skills
    switch (player->getClass())
    {
    case CLASS_DRUID:
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_2H_MACES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_POLEARMS);
        SetMaxSkill(player, SKILL_FIST_WEAPONS);
        break;
    case CLASS_WARRIOR:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_AXES);
        SetMaxSkill(player, SKILL_BOWS);
        SetMaxSkill(player, SKILL_GUNS);
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_2H_SWORDS);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_2H_MACES);
        SetMaxSkill(player, SKILL_2H_AXES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_CROSSBOWS);
        SetMaxSkill(player, SKILL_POLEARMS);
        SetMaxSkill(player, SKILL_FIST_WEAPONS);
        SetMaxSkill(player, SKILL_THROWN);
        break;
    case CLASS_PALADIN:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_AXES);
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_2H_SWORDS);
        SetMaxSkill(player, SKILL_2H_MACES);
        SetMaxSkill(player, SKILL_2H_AXES);
        SetMaxSkill(player, SKILL_POLEARMS);
        break;
    case CLASS_PRIEST:
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_WANDS);
        break;
    case CLASS_SHAMAN:
        SetMaxSkill(player, SKILL_AXES);
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_2H_MACES);
        SetMaxSkill(player, SKILL_2H_AXES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_FIST_WEAPONS);
        break;
    case CLASS_MAGE:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_WANDS);
        break;
    case CLASS_WARLOCK:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_WANDS);
        break;
    case CLASS_HUNTER:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_AXES);
        SetMaxSkill(player, SKILL_BOWS);
        SetMaxSkill(player, SKILL_GUNS);
        SetMaxSkill(player, SKILL_2H_SWORDS);
        SetMaxSkill(player, SKILL_STAVES);
        SetMaxSkill(player, SKILL_2H_AXES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_CROSSBOWS);
        SetMaxSkill(player, SKILL_POLEARMS);
        SetMaxSkill(player, SKILL_FIST_WEAPONS);
        SetMaxSkill(player, SKILL_THROWN);
        break;
    case CLASS_ROGUE:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_AXES);
        SetMaxSkill(player, SKILL_BOWS);
        SetMaxSkill(player, SKILL_GUNS);
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_DAGGERS);
        SetMaxSkill(player, SKILL_CROSSBOWS);
        SetMaxSkill(player, SKILL_FIST_WEAPONS);
        SetMaxSkill(player, SKILL_THROWN);
        break;
#if EXPANSION == 2
    case CLASS_DEATH_KNIGHT:
        SetMaxSkill(player, SKILL_SWORDS);
        SetMaxSkill(player, SKILL_AXES);
        SetMaxSkill(player, SKILL_MACES);
        SetMaxSkill(player, SKILL_2H_SWORDS);
        SetMaxSkill(player, SKILL_2H_MACES);
        SetMaxSkill(player, SKILL_2H_AXES);
        SetMaxSkill(player, SKILL_POLEARMS);
        break;
#endif
    }
}

void GrantMount(Player *player)
{
    if (player->GetTeam() == ALLIANCE)
    {
        player->StoreNewItemInInventorySlot(2411, 1);
    }
    else if (player->GetTeam() == HORDE)
    {
        player->StoreNewItemInInventorySlot(5668, 1);
    }
}

void LearnArmorSkills(Player *player)
{
    switch (player->getClass())
    {
    case CLASS_WARRIOR:
    case CLASS_PALADIN:
        player->SetSkill(SKILL_PLATE_MAIL, 1, 1);
        break;
    case CLASS_SHAMAN:
    case CLASS_HUNTER:
        player->SetSkill(SKILL_MAIL, 1, 1);
        break;
    }
}

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
        player->ADD_GOSSIP_ITEM_EXTENDED(3, "skip to 60", GOSSIP_SENDER_MAIN, 14, "Are you sure?", 0, false);
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
        LearnAvailableClassSpells(player, true);
        LearnWeaponSkills(player);
        LearnArmorSkills(player);
        GrantMount(player);
        player->SetSkill(95, 300, player->GetSkillMax(95), 0);
        for (uint32 itemEntry : Level1StartingGear)
            player->DestroyItemCount(itemEntry, 200, true, false);
        if (player->getClass() == 1) // Warrior
        {
            player->EquipNewItem(0, 13404, 1);  // Head
            player->EquipNewItem(1, 15411, 1);  // Neck
            player->EquipNewItem(2, 12927, 1);  // Shoulders
            player->EquipNewItem(4, 11726, 1);  // Chest
            player->EquipNewItem(5, 13142, 1);  // Belt
            player->EquipNewItem(6, 14554, 1);  // Legs
            player->EquipNewItem(7, 14616, 1);  // Boots
            player->EquipNewItem(8, 12936, 1);  // Bracer
            player->EquipNewItem(9, 14551, 1);  // Gloves
            player->EquipNewItem(10, 17713, 1); // Ring1
            player->EquipNewItem(11, 13098, 1); // Ring2
            player->EquipNewItem(12, 13965, 1); // Trinket1
            player->EquipNewItem(13, 11815, 1); // Trinket2
            player->EquipNewItem(14, 13340, 1); // Cloak
            player->EquipNewItem(15, 12940, 1); // Mainhand
            player->EquipNewItem(16, 12939, 1); // Offhand
            player->EquipNewItem(17, 12651, 1); // Ranged
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 2) // Paladin
        {
            player->EquipNewItem(0, 13404, 1);  // Head
            player->EquipNewItem(1, 15411, 1);  // Neck
            player->EquipNewItem(2, 12927, 1);  // Shoulders
            player->EquipNewItem(4, 11726, 1);  // Chest
            player->EquipNewItem(5, 13142, 1);  // Belt
            player->EquipNewItem(6, 14554, 1);  // Legs
            player->EquipNewItem(7, 14616, 1);  // Boots
            player->EquipNewItem(8, 12936, 1);  // Bracer
            player->EquipNewItem(9, 13957, 1);  // Gloves
            player->EquipNewItem(10, 12548, 1); // Ring1
            player->EquipNewItem(11, 13098, 1); // Ring2
            player->EquipNewItem(12, 13965, 1); // Trinket1
            player->EquipNewItem(13, 11815, 1); // Trinket2
            player->EquipNewItem(14, 13340, 1); // Cloak
            player->EquipNewItem(15, 12784, 1); // Mainhand
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->StoreNewItemInInventorySlot(11784, 1);
            player->StoreNewItemInInventorySlot(12602, 1);
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 3) // Hunter
        {
            player->EquipNewItem(0, 13404, 1);  // Head
            player->EquipNewItem(1, 15411, 1);  // Neck
            player->EquipNewItem(2, 12927, 1);  // Shoulders
            player->EquipNewItem(4, 11726, 1);  // Chest
            player->EquipNewItem(5, 14502, 1);  // Belt
            player->EquipNewItem(6, 15062, 1);  // Legs
            player->EquipNewItem(7, 13967, 1);  // Boots
            player->EquipNewItem(8, 13211, 1);  // Bracer
            player->EquipNewItem(9, 15063, 1);  // Gloves
            player->EquipNewItem(10, 17713, 1); // Ring1
            player->EquipNewItem(11, 13098, 1); // Ring2
            player->EquipNewItem(12, 13965, 1); // Trinket1
            player->EquipNewItem(13, 11815, 1); // Trinket2
            player->EquipNewItem(14, 13340, 1); // Cloak
            player->EquipNewItem(15, 12940, 1); // Mainhand
            player->EquipNewItem(16, 12939, 1); // Offhand
            player->EquipNewItem(17, 18738, 1); // Ranged
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 19319, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 4) // Rogue
        {
            player->EquipNewItem(0, 13404, 1);  // Head
            player->EquipNewItem(1, 15411, 1);  // Neck
            player->EquipNewItem(2, 12927, 1);  // Shoulders
            player->EquipNewItem(4, 14637, 1);  // Chest
            player->EquipNewItem(5, 13252, 1);  // Belt
            player->EquipNewItem(6, 15062, 1);  // Legs
            player->EquipNewItem(7, 12553, 1);  // Boots
            player->EquipNewItem(8, 13120, 1);  // Bracer
            player->EquipNewItem(9, 15063, 1);  // Gloves
            player->EquipNewItem(10, 13098, 1); // Ring1
            player->EquipNewItem(11, 17713, 1); // Ring2
            player->EquipNewItem(12, 13965, 1); // Trinket1
            player->EquipNewItem(13, 11815, 1); // Trinket2
            player->EquipNewItem(14, 13340, 1); // Cloak
            player->EquipNewItem(15, 12940, 1); // Mainhand
            player->EquipNewItem(16, 12939, 1); // Offhand
            player->EquipNewItem(17, 12651, 1); // Ranged
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 5) // Priest
        {
            player->EquipNewItem(0, 22267, 1);  // Head
            player->EquipNewItem(1, 18691, 1);  // Neck
            player->EquipNewItem(2, 14112, 1);  // Shoulders
            player->EquipNewItem(4, 14136, 1);  // Chest
            player->EquipNewItem(5, 11662, 1);  // Belt
            player->EquipNewItem(6, 13170, 1);  // Legs
            player->EquipNewItem(7, 18735, 1);  // Boots
            player->EquipNewItem(8, 11766, 1);  // Bracer
            player->EquipNewItem(9, 13253, 1);  // Gloves
            player->EquipNewItem(10, 12545, 1); // Ring1
            player->EquipNewItem(11, 13001, 1); // Ring2
            player->EquipNewItem(12, 12930, 1); // Trinket1
            player->EquipNewItem(13, 11819, 1); // Trinket2
            player->EquipNewItem(14, 13386, 1); // Cloak
            player->EquipNewItem(15, 13349, 1); // Mainhand
            player->EquipNewItem(16, 10796, 1); // Offhand
            player->EquipNewItem(17, 13396, 1); // Ranged
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 7) // Shaman Ele
        {
            player->EquipNewItem(0, 22267, 1);  // Head
            player->EquipNewItem(1, 12103, 1);  // Neck
            player->EquipNewItem(2, 13013, 1);  // Shoulders
            player->EquipNewItem(4, 11924, 1);  // Chest
            player->EquipNewItem(5, 11662, 1);  // Belt
            player->EquipNewItem(6, 13170, 1);  // Legs
            player->EquipNewItem(7, 13954, 1);  // Boots
            player->EquipNewItem(8, 11765, 1);  // Bracer
            player->EquipNewItem(9, 13253, 1);  // Gloves
            player->EquipNewItem(10, 12545, 1); // Ring1
            player->EquipNewItem(11, 13001, 1); // Ring2
            player->EquipNewItem(12, 12930, 1); // Trinket1
            player->EquipNewItem(13, 13968, 1); // Trinket2
            player->EquipNewItem(14, 15421, 1); // Cloak
            player->EquipNewItem(15, 13964, 1); // Mainhand
            player->EquipNewItem(16, 11904, 1); // Offhand
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->StoreNewItemInInventorySlot(5175, 1);
            player->StoreNewItemInInventorySlot(5176, 1);
            player->StoreNewItemInInventorySlot(5177, 1);
            player->StoreNewItemInInventorySlot(5178, 1);
        }
        else if (player->getClass() == CLASS_MAGE) // Mage
        {
            player->EquipNewItem(0, 22267, 1);  // Head
            player->EquipNewItem(1, 12103, 1);  // Neck
            player->EquipNewItem(2, 11782, 1);  // Shoulders
            player->EquipNewItem(4, 14152, 1);  // Chest
            player->EquipNewItem(5, 11662, 1);  // Belt
            player->EquipNewItem(6, 13170, 1);  // Legs
            player->EquipNewItem(7, 11822, 1);  // Boots
            player->EquipNewItem(8, 11766, 1);  // Bracer
            player->EquipNewItem(9, 13253, 1);  // Gloves
            player->EquipNewItem(10, 942, 1);   // Ring1
            player->EquipNewItem(11, 942, 1);   // Ring2
            player->EquipNewItem(12, 13968, 1); // Trinket1
            player->EquipNewItem(13, 12930, 1); // Trinket2
            player->EquipNewItem(14, 13386, 1); // Cloak
            player->EquipNewItem(15, 13964, 1); // Mainhand
            player->EquipNewItem(16, 10796, 1); // Offhand
            player->EquipNewItem(17, 13938, 1); // Ranged
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 9) // Warlock
        {
            player->EquipNewItem(0, 22267, 1);  // Head
            player->EquipNewItem(1, 18691, 1);  // Neck
            player->EquipNewItem(2, 14112, 1);  // Shoulders
            player->EquipNewItem(4, 14153, 1);  // Chest
            player->EquipNewItem(5, 11662, 1);  // Belt
            player->EquipNewItem(6, 13170, 1);  // Legs
            player->EquipNewItem(7, 18735, 1);  // Boots
            player->EquipNewItem(8, 11766, 1);  // Bracer
            player->EquipNewItem(9, 13253, 1);  // Gloves
            player->EquipNewItem(10, 12545, 1); // Ring1
            player->EquipNewItem(11, 13001, 1); // Ring2
            player->EquipNewItem(12, 12930, 1); // Trinket1
            player->EquipNewItem(13, 13968, 1); // Trinket2
            player->EquipNewItem(14, 13386, 1); // Cloak
            player->EquipNewItem(15, 13964, 1); // Mainhand
            player->EquipNewItem(16, 10796, 1); // Offhand
            player->EquipNewItem(17, 13396, 1); // Ranged
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        else if (player->getClass() == 11) // Druid Feral
        {
            player->EquipNewItem(0, 8345, 1);   // Head
            player->EquipNewItem(1, 15411, 1);  // Neck
            player->EquipNewItem(2, 12927, 1);  // Shoulders
            player->EquipNewItem(4, 14637, 1);  // Chest
            player->EquipNewItem(5, 13252, 1);  // Belt
            player->EquipNewItem(6, 15062, 1);  // Legs
            player->EquipNewItem(7, 12553, 1);  // Boots
            player->EquipNewItem(8, 16710, 1);  // Bracer
            player->EquipNewItem(9, 15063, 1);  // Gloves
            player->EquipNewItem(10, 17713, 1); // Ring1
            player->EquipNewItem(11, 2246, 1);  // Ring2
            player->EquipNewItem(12, 13965, 1); // Trinket1
            player->EquipNewItem(13, 11815, 1); // Trinket2
            player->EquipNewItem(14, 13340, 1); // Cloak
            player->EquipNewItem(15, 11921, 1); // Mainhand
            player->EquipNewItem(19, 16057, 1); // Bag1
            player->EquipNewItem(20, 16057, 1); // Bag2
            player->EquipNewItem(21, 16057, 1); // Bag3
            player->EquipNewItem(22, 16057, 1); // Bag4
            player->SaveInventoryAndGoldToDB();
        }
        player->SetHealthPercent(100);
        if (player->GetPowerType() == POWER_MANA)
        {
            player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
        }
        // if (player->getClass() == 6)     //Deathknight
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
