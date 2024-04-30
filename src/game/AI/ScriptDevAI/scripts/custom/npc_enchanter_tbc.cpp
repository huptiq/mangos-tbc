#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"
#include <string>

#define COST_BRACKET_4 1500000 // 150 gold

std::string constructCostString_tbc(int cost)
{
    if (cost < 100)
    {
        return std::to_string(cost) + "c";
    }
    else if (cost < 10000)
    {
        return std::to_string(cost / 100) + "s";
    }
    else
    {
        return std::to_string(cost / 10000) + "g";
    }
}

int costBrackets_tbc[] = {COST_BRACKET_4};

bool check_wep_for_2h_tbc(Player *player, Item *item)
{
    if (!item || !item->GetProto() ||
        (item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 &&
         item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
         item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
         item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM &&
         item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_STAFF))
    {
        player->GetSession()->SendNotification("Two-handed weapon required");
        player->CLOSE_GOSSIP_MENU();
        return false;
    }
    return true;
}

bool check_for_shield_tbc(Player *player, Item *item)
{
    if (item && item->GetProto()->SubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        player->GetSession()->SendNotification("A shield is required");
        player->CLOSE_GOSSIP_MENU();
        return false;
    }
    return true;
}

enum Enchants
{
    Enchant2HWeapon_MajorAgility = 20,
    Enchant2HWeapon_Savagery,
    EnchantBoots_BoarsSpeed,
    EnchantBoots_CatsSwiftness,
    EnchantBoots_Dexterity,
    EnchantBoots_Fortitude,
    EnchantBoots_Surefooted,
    EnchantBoots_Vitality,
    EnchantBracer_Brawn,
    EnchantBracer_Fortitude,
    EnchantBracer_MajorDefense,
    EnchantBracer_MajorIntellect,
    EnchantBracer_RestoreManaPrime,
    EnchantBracer_Spellpower,
    EnchantBracer_Stats,
    EnchantBracer_SuperiorHealing,
    EnchantChest_ExceptionalHealth,
    EnchantChest_ExceptionalStats,
    EnchantChest_MajorResilience,
    EnchantChest_MajorSpirit,
    EnchantCloak_GreaterAgility,
    EnchantCloak_GreaterArcaneResistance,
    EnchantCloak_GreaterFireResistance,
    EnchantCloak_GreaterNatureResistance,
    EnchantCloak_GreaterShadowResistance,
    EnchantCloak_MajorArmor,
    EnchantCloak_MajorResistance,
    EnchantCloak_SpellPenetration,
    EnchantCloak_Stealth,
    EnchantCloak_Subtlety,
    EnchantGloves_Assault,
    EnchantGloves_Blasting,
    EnchantGloves_MajorHealing,
    EnchantGloves_MajorSpellpower,
    EnchantGloves_MajorStrength,
    EnchantGloves_SpellStrike,
    EnchantGloves_SuperiorAgility,
    EnchantGloves_Threat,
    EnchantShield_FrostResistance,
    EnchantShield_Intellect,
    EnchantShield_MajorStamina,
    EnchantShield_Resistance,
    EnchantShield_ShieldBlock,
    EnchantWeapon_Battlemaster,
    EnchantWeapon_GreaterAgility,
    EnchantWeapon_MajorHealing,
    EnchantWeapon_MajorIntellect,
    EnchantWeapon_MajorSpellpower,
    EnchantWeapon_MajorStriking,
    EnchantWeapon_MightySpirit,
    EnchantWeapon_Mongoose,
    EnchantWeapon_Potency,
    EnchantWeapon_Soulfrost,
    EnchantWeapon_Spellsurge,
    EnchantWeapon_Sunfire,
};

void Enchant_tbc(Player *player, Item *item, uint32 enchantid)
{
    if (!item)
    {
        player->GetSession()->SendNotification("You must first equip the item you are trying to enchant");
        player->CLOSE_GOSSIP_MENU();
        return;
    }

    if (!enchantid)
    {
        player->GetSession()->SendNotification("Something went wrong");
        player->CLOSE_GOSSIP_MENU();
        return;
    }

    item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
    item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
    player->GetSession()->SendNotification("%s succesfully enchanted", item->GetProto()->Name1);
}

void handleEnchant_tbc(Player *player, uint32 enchantId, uint32 costBracket, uint32 slot, const std::string &caseName)
{
    Item *item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    std::string casePrefix = caseName.substr(0, 12);
    if (!item)
    {
        player->GetSession()->SendNotification("You must first equip the item you are trying to enchant");
        return;
    }
    if (player->GetMoney() < costBracket)
    {
        player->GetSession()->SendNotification("You don't have enough money");
        player->CLOSE_GOSSIP_MENU();
        return;
    }
    if (casePrefix == "Enchant2HW" && slot == EQUIPMENT_SLOT_MAINHAND && !check_wep_for_2h_tbc(player, item))
    {
        return;
    }
    if (casePrefix == "EnchantShi" && slot == EQUIPMENT_SLOT_OFFHAND && !check_for_shield_tbc(player, item))
    {
        return;
    }
    if (item)
    {
        player->ModifyMoney(-costBracket);
        Enchant_tbc(player, item, enchantId);
        player->CLOSE_GOSSIP_MENU();
    }
}

bool GossipHello_EnchantNPC_tbc(Player *player, Creature *creature)
{
    if (player->GetLevel() >= 60)
    {
        player->ADD_GOSSIP_ITEM(5, " Chest", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_CHEST);
        player->ADD_GOSSIP_ITEM(5, " Cloak", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_BACK);
        player->ADD_GOSSIP_ITEM(5, " Bracers", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_WRISTS);
        player->ADD_GOSSIP_ITEM(5, " Gloves", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_HANDS);
        player->ADD_GOSSIP_ITEM(5, " Boots", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_FEET);
        player->ADD_GOSSIP_ITEM(5, " Weapon", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_MAINHAND);
        player->ADD_GOSSIP_ITEM(5, " Shield", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_OFFHAND);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_EnchantNPC_tbc(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if (player->GetLevel() >= 1)
    {
        if (sender != GOSSIP_SENDER_MAIN)
            return true;

        if (action < 20)
        {
            switch (action)
            {
            case EQUIPMENT_SLOT_CHEST:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Exceptional Health", GOSSIP_SENDER_MAIN, EnchantChest_ExceptionalHealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Exceptional Stats", GOSSIP_SENDER_MAIN, EnchantChest_ExceptionalStats);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Resilience", GOSSIP_SENDER_MAIN, EnchantChest_MajorResilience);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Spirit", GOSSIP_SENDER_MAIN, EnchantChest_MajorSpirit);
                break;
            case EQUIPMENT_SLOT_BACK:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Greater Agility", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Greater Arcane Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterArcaneResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Greater Fire Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterFireResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Greater Nature Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterNatureResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Greater Shadow Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterShadowResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Armor", GOSSIP_SENDER_MAIN, EnchantCloak_MajorArmor);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_MajorResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Spell Penetration", GOSSIP_SENDER_MAIN, EnchantCloak_SpellPenetration);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Stealth", GOSSIP_SENDER_MAIN, EnchantCloak_Stealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Subtlety", GOSSIP_SENDER_MAIN, EnchantCloak_Subtlety);
                break;
            case EQUIPMENT_SLOT_WRISTS:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Brawn", GOSSIP_SENDER_MAIN, EnchantBracer_Brawn);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Fortitude", GOSSIP_SENDER_MAIN, EnchantBracer_Fortitude);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Defense", GOSSIP_SENDER_MAIN, EnchantBracer_MajorDefense);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Intellect", GOSSIP_SENDER_MAIN, EnchantBracer_MajorIntellect);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Restore Mana Prime", GOSSIP_SENDER_MAIN, EnchantBracer_RestoreManaPrime);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Spellpower", GOSSIP_SENDER_MAIN, EnchantBracer_Spellpower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Stats", GOSSIP_SENDER_MAIN, EnchantBracer_Stats);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Superior Healing", GOSSIP_SENDER_MAIN, EnchantBracer_SuperiorHealing);
                break;
            case EQUIPMENT_SLOT_HANDS:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Assault", GOSSIP_SENDER_MAIN, EnchantGloves_Assault);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Blasting", GOSSIP_SENDER_MAIN, EnchantGloves_Blasting);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Healing", GOSSIP_SENDER_MAIN, EnchantGloves_MajorHealing);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Spellpower", GOSSIP_SENDER_MAIN, EnchantGloves_MajorSpellpower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Strength", GOSSIP_SENDER_MAIN, EnchantGloves_MajorStrength);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Spell Strike", GOSSIP_SENDER_MAIN, EnchantGloves_SpellStrike);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Superior Agility", GOSSIP_SENDER_MAIN, EnchantGloves_SuperiorAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Threat", GOSSIP_SENDER_MAIN, EnchantGloves_Threat);
                break;
            case EQUIPMENT_SLOT_FEET:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Boar's Speed", GOSSIP_SENDER_MAIN, EnchantBoots_BoarsSpeed);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Cat's Swiftness", GOSSIP_SENDER_MAIN, EnchantBoots_CatsSwiftness);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Dexterity", GOSSIP_SENDER_MAIN, EnchantBoots_Dexterity);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Fortitude", GOSSIP_SENDER_MAIN, EnchantBoots_Fortitude);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Surefooted", GOSSIP_SENDER_MAIN, EnchantBoots_Surefooted);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Vitality", GOSSIP_SENDER_MAIN, EnchantBoots_Vitality);
                break;
            case EQUIPMENT_SLOT_MAINHAND:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Agility", GOSSIP_SENDER_MAIN, Enchant2HWeapon_MajorAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Savagery", GOSSIP_SENDER_MAIN, Enchant2HWeapon_Savagery);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Battlemaster", GOSSIP_SENDER_MAIN, EnchantWeapon_Battlemaster);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Greater Agility", GOSSIP_SENDER_MAIN, EnchantWeapon_GreaterAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Healing", GOSSIP_SENDER_MAIN, EnchantWeapon_MajorHealing);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Intellect", GOSSIP_SENDER_MAIN, EnchantWeapon_MajorIntellect);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Spellpower", GOSSIP_SENDER_MAIN, EnchantWeapon_MajorSpellpower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Striking", GOSSIP_SENDER_MAIN, EnchantWeapon_MajorStriking);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Mighty Spirit", GOSSIP_SENDER_MAIN, EnchantWeapon_MightySpirit);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Mongoose", GOSSIP_SENDER_MAIN, EnchantWeapon_Mongoose);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Potency", GOSSIP_SENDER_MAIN, EnchantWeapon_Potency);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Soulfrost", GOSSIP_SENDER_MAIN, EnchantWeapon_Soulfrost);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Spellsurge", GOSSIP_SENDER_MAIN, EnchantWeapon_Spellsurge);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Sunfire", GOSSIP_SENDER_MAIN, EnchantWeapon_Sunfire);
                break;
            case EQUIPMENT_SLOT_OFFHAND:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Frost Resistance", GOSSIP_SENDER_MAIN, EnchantShield_FrostResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Intellect", GOSSIP_SENDER_MAIN, EnchantShield_Intellect);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Major Stamina", GOSSIP_SENDER_MAIN, EnchantShield_MajorStamina);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Resistance", GOSSIP_SENDER_MAIN, EnchantShield_Resistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString_tbc(COST_BRACKET_4) + " Shield Block", GOSSIP_SENDER_MAIN, EnchantShield_ShieldBlock);
                break;
            }
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
        }
        else
        {
            Item *item = nullptr;
            uint32 id = 0;
            switch (action)
            {
            case Enchant2HWeapon_MajorAgility:
                handleEnchant_tbc(player, 2670, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_MajorAgility");
                break;
            case Enchant2HWeapon_Savagery:
                handleEnchant_tbc(player, 2667, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_Savagery");
            case EnchantBoots_BoarsSpeed:
                handleEnchant_tbc(player, 2940, COST_BRACKET_4, EQUIPMENT_SLOT_FEET, "EnchantBoots_BoarsSpeed");
                break;
            case EnchantBoots_CatsSwiftness:
                handleEnchant_tbc(player, 2939, COST_BRACKET_4, EQUIPMENT_SLOT_FEET, "EnchantBoots_CatsSwiftness");
                break;
            case EnchantBoots_Dexterity:
                handleEnchant_tbc(player, 2657, COST_BRACKET_4, EQUIPMENT_SLOT_FEET, "EnchantBoots_Dexterity");
                break;
            case EnchantBoots_Fortitude:
                handleEnchant_tbc(player, 2649, COST_BRACKET_4, EQUIPMENT_SLOT_FEET, "EnchantBoots_Fortitude");
                break;
            case EnchantBoots_Surefooted:
                handleEnchant_tbc(player, 2658, COST_BRACKET_4, EQUIPMENT_SLOT_FEET, "EnchantBoots_Surefooted");
                break;
            case EnchantBoots_Vitality:
                handleEnchant_tbc(player, 2656, COST_BRACKET_4, EQUIPMENT_SLOT_FEET, "EnchantBoots_Vitality");
                break;
            case EnchantBracer_Brawn:
                handleEnchant_tbc(player, 2647, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Brawn");
                break;
            case EnchantBracer_Fortitude:
                handleEnchant_tbc(player, 2649, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Fortitude");
                break;
            case EnchantBracer_MajorDefense:
                handleEnchant_tbc(player, 2648, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MajorDefense");
                break;
            case EnchantBracer_MajorIntellect:
                handleEnchant_tbc(player, 369, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MajorIntellect");
                break;
            case EnchantBracer_RestoreManaPrime:
                handleEnchant_tbc(player, 2679, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_RestoreManaPrime");
                break;
            case EnchantBracer_Spellpower:
                handleEnchant_tbc(player, 2650, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Spellpower");
                break;
            case EnchantBracer_Stats:
                handleEnchant_tbc(player, 1891, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Stats");
                break;
            case EnchantBracer_SuperiorHealing:
                handleEnchant_tbc(player, 2617, COST_BRACKET_4, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_SuperiorHealing");
                break;
            case EnchantChest_ExceptionalHealth:
                handleEnchant_tbc(player, 2659, COST_BRACKET_4, EQUIPMENT_SLOT_CHEST, "EnchantChest_ExceptionalHealth");
                break;
            case EnchantChest_ExceptionalStats:
                handleEnchant_tbc(player, 2661, COST_BRACKET_4, EQUIPMENT_SLOT_CHEST, "EnchantChest_ExceptionalStats");
                break;
            case EnchantChest_MajorResilience:
                handleEnchant_tbc(player, 2933, COST_BRACKET_4, EQUIPMENT_SLOT_CHEST, "EnchantChest_MajorResilience");
                break;
            case EnchantChest_MajorSpirit:
                handleEnchant_tbc(player, 1144, COST_BRACKET_4, EQUIPMENT_SLOT_CHEST, "EnchantChest_MajorSpirit");
                break;
            case EnchantCloak_GreaterAgility:
                handleEnchant_tbc(player, 368, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterAgility");
                break;
            case EnchantCloak_GreaterArcaneResistance:
                handleEnchant_tbc(player, 1257, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterArcaneResistance");
                break;
            case EnchantCloak_GreaterFireResistance:
                handleEnchant_tbc(player, 2619, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterFireResistance");
                break;
            case EnchantCloak_GreaterNatureResistance:
                handleEnchant_tbc(player, 2620, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterNatureResistance");
                break;
            case EnchantCloak_GreaterShadowResistance:
                handleEnchant_tbc(player, 1441, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterShadowResistance");
                break;
            case EnchantCloak_MajorArmor:
                handleEnchant_tbc(player, 2662, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_MajorArmor");
                break;
            case EnchantCloak_MajorResistance:
                handleEnchant_tbc(player, 2664, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_MajorResistance");
                break;
            case EnchantCloak_SpellPenetration:
                handleEnchant_tbc(player, 2938, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_SpellPenetration");
                break;
            case EnchantCloak_Stealth:
                handleEnchant_tbc(player, 910, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_Stealth");
                break;
            case EnchantCloak_Subtlety:
                handleEnchant_tbc(player, 2621, COST_BRACKET_4, EQUIPMENT_SLOT_BACK, "EnchantCloak_Subtlety");
                break;
            case EnchantGloves_Assault:
                handleEnchant_tbc(player, 1594, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Assault");
                break;
            case EnchantGloves_Blasting:
                handleEnchant_tbc(player, 2934, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Blasting");
                break;
            case EnchantGloves_MajorHealing:
                handleEnchant_tbc(player, 2322, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_MajorHealing");
                break;
            case EnchantGloves_MajorSpellpower:
                handleEnchant_tbc(player, 2937, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_MajorSpellpower");
                break;
            case EnchantGloves_MajorStrength:
                handleEnchant_tbc(player, 684, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_MajorStrength");
                break;
            case EnchantGloves_SpellStrike:
                handleEnchant_tbc(player, 2935, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_SpellStrike");
                break;
            case EnchantGloves_SuperiorAgility:
                handleEnchant_tbc(player, 2564, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_SuperiorAgility");
                break;
            case EnchantGloves_Threat:
                handleEnchant_tbc(player, 2613, COST_BRACKET_4, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Threat");
                break;
            case EnchantShield_FrostResistance:
                handleEnchant_tbc(player, 926, COST_BRACKET_4, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_FrostResistance");
                break;
            case EnchantShield_Intellect:
                handleEnchant_tbc(player, 2654, COST_BRACKET_4, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_Intellect");
                break;
            case EnchantShield_MajorStamina:
                handleEnchant_tbc(player, 1071, COST_BRACKET_4, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_MajorStamina");
                break;
            case EnchantShield_Resistance:
                handleEnchant_tbc(player, 1888, COST_BRACKET_4, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_Resistance");
                break;
            case EnchantShield_ShieldBlock:
                handleEnchant_tbc(player, 2655, COST_BRACKET_4, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_ShieldBlock");
                break;
            case EnchantWeapon_Battlemaster:
                handleEnchant_tbc(player, 2675, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Battlemaster");
                break;
            case EnchantWeapon_GreaterAgility:
                handleEnchant_tbc(player, 3222, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_GreaterAgility");
                break;
            case EnchantWeapon_MajorHealing:
                handleEnchant_tbc(player, 2343, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MajorHealing");
                break;
            case EnchantWeapon_MajorIntellect:
                handleEnchant_tbc(player, 2666, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MajorIntellect");
                break;
            case EnchantWeapon_MajorSpellpower:
                handleEnchant_tbc(player, 2669, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MajorSpellpower");
                break;
            case EnchantWeapon_MajorStriking:
                handleEnchant_tbc(player, 963, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MajorStriking");
                break;
            case EnchantWeapon_MightySpirit:
                handleEnchant_tbc(player, 2567, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MightySpirit");
                break;
            case EnchantWeapon_Mongoose:
                handleEnchant_tbc(player, 2673, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Mongoose");
                break;
            case EnchantWeapon_Potency:
                handleEnchant_tbc(player, 2668, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Potency");
                break;
            case EnchantWeapon_Soulfrost:
                handleEnchant_tbc(player, 2672, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Soulfrost");
                break;
            case EnchantWeapon_Spellsurge:
                handleEnchant_tbc(player, 2674, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Spellsurge");
                break;
            case EnchantWeapon_Sunfire:
                handleEnchant_tbc(player, 2671, COST_BRACKET_4, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Sunfire");
                break;
            }
        }
    }
    return true;
}

void AddSC_npc_enchanter_tbc()
{
    auto newscript = new Script;
    newscript->Name = "custom_enchant_npc_tbc";
    newscript->pGossipHello = &GossipHello_EnchantNPC_tbc;
    newscript->pGossipSelect = &GossipSelect_EnchantNPC_tbc;
    newscript->RegisterSelf(false);
}
