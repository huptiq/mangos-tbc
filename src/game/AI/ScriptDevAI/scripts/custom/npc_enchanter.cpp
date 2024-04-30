#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"
#include <string>

#define COST_BRACKET_1 6000   // 60 silver
#define COST_BRACKET_2 60000  // 6 gold
#define COST_BRACKET_3 500000 // 50 gold

std::string constructCostString(int cost)
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

int costBrackets[] = {COST_BRACKET_1, COST_BRACKET_2, COST_BRACKET_3};

bool check_wep_for_2h(Player *player, Item *item)
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

bool check_for_shield(Player *player, Item *item)
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
    EnchantChest_MinorHealth = 20,
    EnchantChest_MinorMana,
    EnchantChest_MinorAbsorption,
    EnchantCloak_MinorResistance,
    EnchantBracer_MinorStamina,
    EnchantChest_LesserHealth,
    EnchantBracer_MinorSpirit,
    EnchantBracer_MinorHealth,
    EnchantCloak_MinorProtection,
    EnchantBracer_MinorDeflect,
    EnchantChest_LesserMana,
    EnchantBracer_MinorAgility,
    EnchantBracer_MinorStrength,
    EnchantWeapon_MinorBeastslayer,
    EnchantWeapon_MinorStriking,
    Enchant2HWeapon_MinorImpact,
    Enchant2HWeapon_LesserIntellect,
    EnchantShield_MinorStamina,
    Enchant2HWeapon_LesserSpirit,
    EnchantCloak_MinorAgility,
    EnchantCloak_LesserProtection,
    EnchantShield_LesserProtection,
    EnchantChest_Health,
    EnchantBracer_LesserSpirit,
    EnchantCloak_LesserFireResistance,
    EnchantBoots_MinorStamina,
    EnchantBoots_MinorAgility,
    EnchantShield_LesserSpirit,
    EnchantBracer_LesserStamina,
    EnchantCloak_LesserShadowResistance,
    EnchantWeapon_LesserStriking,
    EnchantBracer_LesserStrength,
    EnchantChest_LesserAbsorption,
    Enchant2HWeapon_LesserImpact,
    EnchantChest_Mana,
    EnchantGloves_Mining,
    EnchantGloves_Herbalism,
    EnchantGloves_Fishing,
    EnchantBracer_LesserIntellect,
    EnchantChest_MinorStats,
    EnchantShield_LesserStamina,
    EnchantCloak_Defense,
    EnchantBoots_LesserAgility,
    EnchantChest_GreaterHealth,
    EnchantBracer_Spirit,
    EnchantBoots_LesserStamina,
    EnchantBracer_LesserDeflection,
    EnchantBracer_Stamina,
    EnchantWeapon_LesserBeastslayer,
    EnchantWeapon_LesserElementalSlayer,
    EnchantCloak_FireResistance,
    EnchantShield_Spirit,
    EnchantBracer_Strength,
    EnchantChest_GreaterMana,
    EnchantBoots_LesserSpirit,
    EnchantWeapon_WintersMight,
    EnchantShield_LesserBlock,
    EnchantWeapon_Striking,
    Enchant2HWeapon_Impact,
    EnchantGloves_Skinning,
    EnchantChest_LesserStats,
    EnchantCloak_GreaterDefense,
    EnchantCloak_Resistance,
    EnchantGloves_Agility,
    EnchantShield_Stamina,
    EnchantBracer_Intellect,
    EnchantBoots_Stamina,
    EnchantGloves_AdvancedMining,
    EnchantBracer_GreaterSpirit,
    EnchantChest_SuperiorHealth,
    EnchantGloves_AdvancedHerbalism,
    EnchantCloak_LesserAgility,
    EnchantGloves_Strength,
    EnchantBoots_MinorSpeed,
    EnchantShield_GreaterSpirit,
    EnchantWeapon_Demonslaying,
    EnchantChest_SuperiorMana,
    EnchantBracer_Deflection,
    EnchantShield_FrostResistance,
    EnchantBoots_Agility,
    Enchant2HWeapon_GreaterImpact,
    EnchantBracer_GreaterStrength,
    EnchantChest_Stats,
    EnchantWeapon_GreaterStriking,
    EnchantBracer_GreaterStamina,
    EnchantGloves_RidingSkill,
    EnchantGloves_MinorHaste,
    EnchantBracer_GreaterIntellect,
    EnchantBoots_GreaterStamina,
    EnchantWeapon_FieryWeapon,
    EnchantCloak_GreaterResistance,
    EnchantShield_GreaterStamina,
    EnchantBracer_SuperiorSpirit,
    EnchantGloves_GreaterAgility,
    EnchantBoots_Spirit,
    EnchantChest_MajorHealth,
    EnchantShield_SuperiorSpirit,
    EnchantCloak_SuperiorDefense,
    EnchantWeapon_IcyChill,
    EnchantChest_MajorMana,
    EnchantWeapon_Strength,
    EnchantWeapon_Agility,
    EnchantBracer_ManaRegeneration,
    Enchant2HWeapon_Agility,
    EnchantBracer_SuperiorStrength,
    EnchantGloves_GreaterStrength,
    EnchantBoots_GreaterAgility,
    Enchant2HWeapon_SuperiorImpact,
    EnchantWeapon_UnholyWeapon,
    EnchantBracer_SuperiorStamina,
    EnchantChest_GreaterStats,
    EnchantWeapon_SuperiorStriking,
    EnchantWeapon_Lifestealing,
    EnchantWeapon_Crusader,
    Enchant2HWeapon_MajorSpirit,
    Enchant2HWeapon_MajorIntellect,
    EnchantWeapon_SpellPower,
    EnchantWeapon_HealingPower,
    EnchantBracer_HealingPower,
    EnchantWeapon_MightySpirit,
    EnchantWeapon_MightyIntellect,
    EnchantGloves_Threat,
    EnchantGloves_ShadowPower,
    EnchantGloves_FrostPower,
    EnchantGloves_FirePower,
    EnchantGloves_HealingPower,
    EnchantGloves_SuperiorAgility,
    EnchantCloak_GreaterFireResistance,
    EnchantCloak_GreaterNatureResistance,
    EnchantCloak_Stealth,
    EnchantCloak_Subtlety,
    EnchantCloak_Dodge,
};

enum Options
{
    EQUIPMENT_SLOT_MAINHAND_1 = 13,
    EQUIPMENT_SLOT_MAINHAND_2 = 5,
    EQUIPMENT_SLOT_CHEST_1 = 12,
    EQUIPMENT_SLOT_BACK_1 = 11,
    EQUIPMENT_SLOT_WRISTS_1 = 10,
    EQUIPMENT_SLOT_HANDS_1 = 6,
};

void Enchant(Player *player, Item *item, uint32 enchantid)
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

void handleEnchant(Player *player, uint32 enchantId, uint32 costBracket, uint32 slot, const std::string &caseName)
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
    if (casePrefix == "Enchant2HW" && slot == EQUIPMENT_SLOT_MAINHAND && !check_wep_for_2h(player, item))
    {
        return;
    }
    if (casePrefix == "EnchantShi" && slot == EQUIPMENT_SLOT_OFFHAND && !check_for_shield(player, item))
    {
        return;
    }
    if (item)
    {
        player->ModifyMoney(-costBracket);
        Enchant(player, item, enchantId);
        player->CLOSE_GOSSIP_MENU();
    }
}

bool GossipHello_EnchantNPC(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM(5, " Chest I", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_CHEST);
    if (player->GetLevel() >= 40)
    {
        player->ADD_GOSSIP_ITEM(5, " Chest II", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_CHEST_1);
    }
    player->ADD_GOSSIP_ITEM(5, " Cloak I", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_BACK);
    if (player->GetLevel() >= 40)
    {
        player->ADD_GOSSIP_ITEM(5, " Cloak II", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_BACK_1);
    }
    player->ADD_GOSSIP_ITEM(5, " Bracers I", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_WRISTS);
    if (player->GetLevel() >= 20)
    {
        player->ADD_GOSSIP_ITEM(5, " Bracers II", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_WRISTS_1);
    }
    if (player->GetLevel() >= 20)
    {
        player->ADD_GOSSIP_ITEM(5, " Gloves I", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_HANDS);
    }
    if (player->GetLevel() >= 40)
    {
        player->ADD_GOSSIP_ITEM(5, " Gloves II", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_HANDS_1);
    }
    player->ADD_GOSSIP_ITEM(5, " Boots I", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_FEET);
    player->ADD_GOSSIP_ITEM(5, " Weapon I", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_MAINHAND);
    if (player->GetLevel() >= 40)
    {
        player->ADD_GOSSIP_ITEM(5, " Weapon II", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_MAINHAND_1);
        player->ADD_GOSSIP_ITEM(5, " Weapon III", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_MAINHAND_2);
    }
    player->ADD_GOSSIP_ITEM(5, " Shield", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_OFFHAND);
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_EnchantNPC(Player *player, Creature *creature, uint32 sender, uint32 action)
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
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Health", GOSSIP_SENDER_MAIN, EnchantChest_MinorHealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Mana", GOSSIP_SENDER_MAIN, EnchantChest_MinorMana);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Absorption", GOSSIP_SENDER_MAIN, EnchantChest_MinorAbsorption);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Lesser Health", GOSSIP_SENDER_MAIN, EnchantChest_LesserHealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Lesser Mana", GOSSIP_SENDER_MAIN, EnchantChest_LesserMana);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Health", GOSSIP_SENDER_MAIN, EnchantChest_Health);
                if (player->GetLevel() >= 20)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Absorption", GOSSIP_SENDER_MAIN, EnchantChest_LesserAbsorption);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Mana", GOSSIP_SENDER_MAIN, EnchantChest_Mana);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Minor Stats", GOSSIP_SENDER_MAIN, EnchantChest_MinorStats);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Greater Health", GOSSIP_SENDER_MAIN, EnchantChest_GreaterHealth);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Greater Mana", GOSSIP_SENDER_MAIN, EnchantChest_GreaterMana);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Stats", GOSSIP_SENDER_MAIN, EnchantChest_LesserStats);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Superior Health", GOSSIP_SENDER_MAIN, EnchantChest_SuperiorHealth);
                }
                break;
            case EQUIPMENT_SLOT_CHEST_1:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Mana", GOSSIP_SENDER_MAIN, EnchantChest_SuperiorMana);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Stats", GOSSIP_SENDER_MAIN, EnchantChest_Stats);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Major Health", GOSSIP_SENDER_MAIN, EnchantChest_MajorHealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Major Mana", GOSSIP_SENDER_MAIN, EnchantChest_MajorMana);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Stats", GOSSIP_SENDER_MAIN, EnchantChest_GreaterStats);
                break;
            case EQUIPMENT_SLOT_BACK:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_MinorResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Protection", GOSSIP_SENDER_MAIN, EnchantCloak_MinorProtection);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Agility", GOSSIP_SENDER_MAIN, EnchantCloak_MinorAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Lesser Protection", GOSSIP_SENDER_MAIN, EnchantCloak_LesserProtection);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Lesser Fire Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_LesserFireResistance);
                if (player->GetLevel() >= 20)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Shadow Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_LesserShadowResistance);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Defense", GOSSIP_SENDER_MAIN, EnchantCloak_Defense);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Fire Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_FireResistance);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Greater Defense", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterDefense);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_Resistance);
                }
                break;
            case EQUIPMENT_SLOT_BACK_1:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Lesser Agility", GOSSIP_SENDER_MAIN, EnchantCloak_LesserAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Defense", GOSSIP_SENDER_MAIN, EnchantCloak_SuperiorDefense);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Fire Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterFireResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Nature Resistance", GOSSIP_SENDER_MAIN, EnchantCloak_GreaterNatureResistance);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Stealth", GOSSIP_SENDER_MAIN, EnchantCloak_Stealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Subtlety", GOSSIP_SENDER_MAIN, EnchantCloak_Subtlety);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Dodge", GOSSIP_SENDER_MAIN, EnchantCloak_Dodge);
                break;
            case EQUIPMENT_SLOT_WRISTS:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Stamina", GOSSIP_SENDER_MAIN, EnchantBracer_MinorStamina);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Spirit", GOSSIP_SENDER_MAIN, EnchantBracer_MinorSpirit);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Health", GOSSIP_SENDER_MAIN, EnchantBracer_MinorHealth);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Deflect", GOSSIP_SENDER_MAIN, EnchantBracer_MinorDeflect);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Agility", GOSSIP_SENDER_MAIN, EnchantBracer_MinorAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Strength", GOSSIP_SENDER_MAIN, EnchantBracer_MinorStrength);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Lesser Spirit", GOSSIP_SENDER_MAIN, EnchantBracer_LesserSpirit);
                if (player->GetLevel() >= 20)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Stamina", GOSSIP_SENDER_MAIN, EnchantBracer_LesserStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Strength", GOSSIP_SENDER_MAIN, EnchantBracer_LesserStrength);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Intellect", GOSSIP_SENDER_MAIN, EnchantBracer_LesserIntellect);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Spirit", GOSSIP_SENDER_MAIN, EnchantBracer_Spirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Deflection", GOSSIP_SENDER_MAIN, EnchantBracer_LesserDeflection);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Stamina", GOSSIP_SENDER_MAIN, EnchantBracer_Stamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Strength", GOSSIP_SENDER_MAIN, EnchantBracer_Strength);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Intellect", GOSSIP_SENDER_MAIN, EnchantBracer_Intellect);
                }
                break;
            case EQUIPMENT_SLOT_WRISTS_1:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Greater Spirit", GOSSIP_SENDER_MAIN, EnchantBracer_GreaterSpirit);
                if (player->GetLevel() >= 40)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Deflection", GOSSIP_SENDER_MAIN, EnchantBracer_Deflection);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Strength", GOSSIP_SENDER_MAIN, EnchantBracer_GreaterStrength);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Stamina", GOSSIP_SENDER_MAIN, EnchantBracer_GreaterStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Intellect", GOSSIP_SENDER_MAIN, EnchantBracer_GreaterIntellect);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Spirit", GOSSIP_SENDER_MAIN, EnchantBracer_SuperiorSpirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Mana Regeneration", GOSSIP_SENDER_MAIN, EnchantBracer_ManaRegeneration);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Strength", GOSSIP_SENDER_MAIN, EnchantBracer_SuperiorStrength);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Stamina", GOSSIP_SENDER_MAIN, EnchantBracer_SuperiorStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Healing Power", GOSSIP_SENDER_MAIN, EnchantBracer_HealingPower);
                }
                break;
            case EQUIPMENT_SLOT_HANDS:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Mining", GOSSIP_SENDER_MAIN, EnchantGloves_Mining);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Herbalism", GOSSIP_SENDER_MAIN, EnchantGloves_Herbalism);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Fishing", GOSSIP_SENDER_MAIN, EnchantGloves_Fishing);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Skinning", GOSSIP_SENDER_MAIN, EnchantGloves_Skinning);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Agility", GOSSIP_SENDER_MAIN, EnchantGloves_Agility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Advanced Mining", GOSSIP_SENDER_MAIN, EnchantGloves_AdvancedMining);
                break;
            case EQUIPMENT_SLOT_HANDS_1:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Advanced Herbalism", GOSSIP_SENDER_MAIN, EnchantGloves_AdvancedHerbalism);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Strength", GOSSIP_SENDER_MAIN, EnchantGloves_Strength);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Riding Skill", GOSSIP_SENDER_MAIN, EnchantGloves_RidingSkill);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Minor Haste", GOSSIP_SENDER_MAIN, EnchantGloves_MinorHaste);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Agility", GOSSIP_SENDER_MAIN, EnchantGloves_GreaterAgility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Strength", GOSSIP_SENDER_MAIN, EnchantGloves_GreaterStrength);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Threat", GOSSIP_SENDER_MAIN, EnchantGloves_Threat);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Shadow Power", GOSSIP_SENDER_MAIN, EnchantGloves_ShadowPower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Frost Power", GOSSIP_SENDER_MAIN, EnchantGloves_FrostPower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Fire Power", GOSSIP_SENDER_MAIN, EnchantGloves_FirePower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Healing Power", GOSSIP_SENDER_MAIN, EnchantGloves_HealingPower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Agility", GOSSIP_SENDER_MAIN, EnchantGloves_SuperiorAgility);
                break;
            case EQUIPMENT_SLOT_FEET:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Stamina", GOSSIP_SENDER_MAIN, EnchantBoots_MinorStamina);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Agility", GOSSIP_SENDER_MAIN, EnchantBoots_MinorAgility);
                if (player->GetLevel() >= 20)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Agility", GOSSIP_SENDER_MAIN, EnchantBoots_LesserAgility);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Stamina", GOSSIP_SENDER_MAIN, EnchantBoots_LesserStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Spirit", GOSSIP_SENDER_MAIN, EnchantBoots_LesserSpirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Stamina", GOSSIP_SENDER_MAIN, EnchantBoots_Stamina);
                }
                if (player->GetLevel() >= 40)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Minor Speed", GOSSIP_SENDER_MAIN, EnchantBoots_MinorSpeed);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Agility", GOSSIP_SENDER_MAIN, EnchantBoots_Agility);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Stamina", GOSSIP_SENDER_MAIN, EnchantBoots_GreaterStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Spirit", GOSSIP_SENDER_MAIN, EnchantBoots_Spirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Agility", GOSSIP_SENDER_MAIN, EnchantBoots_GreaterAgility);
                }
                break;
            case EQUIPMENT_SLOT_MAINHAND:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Beastslayer", GOSSIP_SENDER_MAIN, EnchantWeapon_MinorBeastslayer);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Striking", GOSSIP_SENDER_MAIN, EnchantWeapon_MinorStriking);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " 2H Minor Impact", GOSSIP_SENDER_MAIN, Enchant2HWeapon_MinorImpact);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " 2H Lesser Intellect", GOSSIP_SENDER_MAIN, Enchant2HWeapon_LesserIntellect);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " 2H Lesser Spirit", GOSSIP_SENDER_MAIN, Enchant2HWeapon_LesserSpirit);
                if (player->GetLevel() >= 20)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Striking", GOSSIP_SENDER_MAIN, EnchantWeapon_LesserStriking);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " 2H Lesser Impact", GOSSIP_SENDER_MAIN, Enchant2HWeapon_LesserImpact);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Beastslayer", GOSSIP_SENDER_MAIN, EnchantWeapon_LesserBeastslayer);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Elemental Slayer", GOSSIP_SENDER_MAIN, EnchantWeapon_LesserElementalSlayer);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Winter's Might", GOSSIP_SENDER_MAIN, EnchantWeapon_WintersMight);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Striking", GOSSIP_SENDER_MAIN, EnchantWeapon_Striking);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " 2H Impact", GOSSIP_SENDER_MAIN, Enchant2HWeapon_Impact);
                }
                break;
            case EQUIPMENT_SLOT_MAINHAND_1:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Demonslaying", GOSSIP_SENDER_MAIN, EnchantWeapon_Demonslaying);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " 2H Greater Impact", GOSSIP_SENDER_MAIN, Enchant2HWeapon_GreaterImpact);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Striking", GOSSIP_SENDER_MAIN, EnchantWeapon_GreaterStriking);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Fiery Weapon", GOSSIP_SENDER_MAIN, EnchantWeapon_FieryWeapon);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Icy Chill", GOSSIP_SENDER_MAIN, EnchantWeapon_IcyChill);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Strength", GOSSIP_SENDER_MAIN, EnchantWeapon_Strength);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Agility", GOSSIP_SENDER_MAIN, EnchantWeapon_Agility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " 2H Agility", GOSSIP_SENDER_MAIN, Enchant2HWeapon_Agility);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " 2H Superior Impact", GOSSIP_SENDER_MAIN, Enchant2HWeapon_SuperiorImpact);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Unholy Weapon", GOSSIP_SENDER_MAIN, EnchantWeapon_UnholyWeapon);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Striking", GOSSIP_SENDER_MAIN, EnchantWeapon_SuperiorStriking);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Lifestealing", GOSSIP_SENDER_MAIN, EnchantWeapon_Lifestealing);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Crusader", GOSSIP_SENDER_MAIN, EnchantWeapon_Crusader);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " 2H Major Spirit", GOSSIP_SENDER_MAIN, Enchant2HWeapon_MajorSpirit);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " 2H Major Intellect", GOSSIP_SENDER_MAIN, Enchant2HWeapon_MajorIntellect);
                break;
            case EQUIPMENT_SLOT_MAINHAND_2:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Spell Power", GOSSIP_SENDER_MAIN, EnchantWeapon_SpellPower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Healing Power", GOSSIP_SENDER_MAIN, EnchantWeapon_HealingPower);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Mighty Spirit", GOSSIP_SENDER_MAIN, EnchantWeapon_MightySpirit);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Mighty Intellect", GOSSIP_SENDER_MAIN, EnchantWeapon_MightyIntellect);
                break;
            case EQUIPMENT_SLOT_OFFHAND:
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Minor Stamina", GOSSIP_SENDER_MAIN, EnchantShield_MinorStamina);
                player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_1) + " Lesser Protection", GOSSIP_SENDER_MAIN, EnchantShield_LesserProtection);
                if (player->GetLevel() >= 20)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Spirit", GOSSIP_SENDER_MAIN, EnchantShield_LesserSpirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Stamina", GOSSIP_SENDER_MAIN, EnchantShield_LesserStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Spirit", GOSSIP_SENDER_MAIN, EnchantShield_Spirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Lesser Block", GOSSIP_SENDER_MAIN, EnchantShield_LesserBlock);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_2) + " Stamina", GOSSIP_SENDER_MAIN, EnchantShield_Stamina);
                }
                if (player->GetLevel() >= 40)
                {
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Spirit", GOSSIP_SENDER_MAIN, EnchantShield_GreaterSpirit);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Frost Resistance", GOSSIP_SENDER_MAIN, EnchantShield_FrostResistance);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Greater Stamina", GOSSIP_SENDER_MAIN, EnchantShield_GreaterStamina);
                    player->ADD_GOSSIP_ITEM(5, "" + constructCostString(COST_BRACKET_3) + " Superior Spirit", GOSSIP_SENDER_MAIN, EnchantShield_SuperiorSpirit);
                }
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
            case EnchantChest_MinorHealth:
                handleEnchant(player, 41, COST_BRACKET_1, EQUIPMENT_SLOT_CHEST, "EnchantChest_MinorHealth");
                break;
            case EnchantChest_MinorMana:
                handleEnchant(player, 24, COST_BRACKET_1, EQUIPMENT_SLOT_CHEST, "EnchantChest_MinorMana");
                break;
            case EnchantChest_MinorAbsorption:
                handleEnchant(player, 44, COST_BRACKET_1, EQUIPMENT_SLOT_CHEST, "EnchantChest_MinorAbsorption");
                break;
            case EnchantCloak_MinorResistance:
                handleEnchant(player, 65, COST_BRACKET_1, EQUIPMENT_SLOT_BACK, "EnchantCloak_MinorResistance");
                break;
            case EnchantBracer_MinorStamina:
                handleEnchant(player, 66, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MinorStamina");
                break;
            case EnchantChest_LesserHealth:
                handleEnchant(player, 242, COST_BRACKET_1, EQUIPMENT_SLOT_CHEST, "EnchantChest_LesserHealth");
                break;
            case EnchantBracer_MinorSpirit:
                handleEnchant(player, 243, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MinorSpirit");
                break;
            case EnchantBracer_MinorHealth:
                handleEnchant(player, 41, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MinorHealth");
                break;
            case EnchantCloak_MinorProtection:
                handleEnchant(player, 783, COST_BRACKET_1, EQUIPMENT_SLOT_BACK, "EnchantCloak_MinorProtection");
                break;
            case EnchantBracer_MinorDeflect:
                handleEnchant(player, 924, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MinorDeflect");
                break;
            case EnchantChest_LesserMana:
                handleEnchant(player, 246, COST_BRACKET_1, EQUIPMENT_SLOT_CHEST, "EnchantChest_LesserMana");
                break;
            case EnchantBracer_MinorAgility:
                handleEnchant(player, 247, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MinorAgility");
                break;
            case EnchantBracer_MinorStrength:
                handleEnchant(player, 248, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_MinorStrength");
                break;
            case EnchantWeapon_MinorBeastslayer:
                handleEnchant(player, 249, COST_BRACKET_1, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MinorBeastslayer");
                break;
            case EnchantWeapon_MinorStriking:
                handleEnchant(player, 250, COST_BRACKET_1, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MinorStriking");
                break;
            case Enchant2HWeapon_MinorImpact:
                handleEnchant(player, 241, COST_BRACKET_1, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_MinorImpact");
                break;
            case Enchant2HWeapon_LesserIntellect:
                handleEnchant(player, 723, COST_BRACKET_1, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_LesserIntellect");
                break;
            case EnchantShield_MinorStamina:
                handleEnchant(player, 66, COST_BRACKET_1, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_MinorStamina");
                break;
            case Enchant2HWeapon_LesserSpirit:
                handleEnchant(player, 255, COST_BRACKET_1, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_LesserSpirit");
                break;
            case EnchantCloak_MinorAgility:
                handleEnchant(player, 247, COST_BRACKET_1, EQUIPMENT_SLOT_BACK, "EnchantCloak_MinorAgility");
                break;
            case EnchantCloak_LesserProtection:
                handleEnchant(player, 744, COST_BRACKET_1, EQUIPMENT_SLOT_BACK, "EnchantCloak_LesserProtection");
                break;
            case EnchantShield_LesserProtection:
                handleEnchant(player, 848, COST_BRACKET_1, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_LesserProtection");
                break;
            case EnchantChest_Health:
                handleEnchant(player, 254, COST_BRACKET_1, EQUIPMENT_SLOT_CHEST, "EnchantChest_Health");
                break;
            case EnchantBracer_LesserSpirit:
                handleEnchant(player, 255, COST_BRACKET_1, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_LesserSpirit");
                break;
            case EnchantCloak_LesserFireResistance:
                handleEnchant(player, 256, COST_BRACKET_1, EQUIPMENT_SLOT_BACK, "EnchantCloak_LesserFireResistance");
                break;
            case EnchantBoots_MinorStamina:
                handleEnchant(player, 66, COST_BRACKET_1, EQUIPMENT_SLOT_FEET, "EnchantBoots_MinorStamina");
                break;
            case EnchantBoots_MinorAgility:
                handleEnchant(player, 247, COST_BRACKET_1, EQUIPMENT_SLOT_FEET, "EnchantBoots_MinorAgility");
                break;
            case EnchantShield_LesserSpirit:
                handleEnchant(player, 255, COST_BRACKET_2, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_LesserSpirit");
                break;
            case EnchantBracer_LesserStamina:
                handleEnchant(player, 724, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_LesserStamina");
                break;
            case EnchantCloak_LesserShadowResistance:
                handleEnchant(player, 804, COST_BRACKET_2, EQUIPMENT_SLOT_BACK, "EnchantCloak_LesserShadowResistance");
                break;
            case EnchantWeapon_LesserStriking:
                handleEnchant(player, 241, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_LesserStriking");
                break;
            case EnchantBracer_LesserStrength:
                handleEnchant(player, 823, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_LesserStrength");
                break;
            case EnchantChest_LesserAbsorption:
                handleEnchant(player, 63, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_LesserAbsorption");
                break;
            case Enchant2HWeapon_LesserImpact:
                handleEnchant(player, 943, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_LesserImpact");
                break;
            case EnchantChest_Mana:
                handleEnchant(player, 843, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_Mana");
                break;
            case EnchantGloves_Mining:
                handleEnchant(player, 844, COST_BRACKET_2, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Mining");
                break;
            case EnchantGloves_Herbalism:
                handleEnchant(player, 845, COST_BRACKET_2, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Herbalism");
                break;
            case EnchantGloves_Fishing:
                handleEnchant(player, 846, COST_BRACKET_2, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Fishing");
                break;
            case EnchantBracer_LesserIntellect:
                handleEnchant(player, 723, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_LesserIntellect");
                break;
            case EnchantChest_MinorStats:
                handleEnchant(player, 847, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_MinorStats");
                break;
            case EnchantShield_LesserStamina:
                handleEnchant(player, 724, COST_BRACKET_2, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_LesserStamina");
                break;
            case EnchantCloak_Defense:
                handleEnchant(player, 848, COST_BRACKET_2, EQUIPMENT_SLOT_BACK, "EnchantCloak_Defense");
                break;
            case EnchantBoots_LesserAgility:
                handleEnchant(player, 849, COST_BRACKET_2, EQUIPMENT_SLOT_FEET, "EnchantBoots_LesserAgility");
                break;
            case EnchantChest_GreaterHealth:
                handleEnchant(player, 850, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_GreaterHealth");
                break;
            case EnchantBracer_Spirit:
                handleEnchant(player, 851, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Spirit");
                break;
            case EnchantBoots_LesserStamina:
                handleEnchant(player, 724, COST_BRACKET_2, EQUIPMENT_SLOT_FEET, "EnchantBoots_LesserStamina");
                break;
            case EnchantBracer_LesserDeflection:
                handleEnchant(player, 925, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_LesserDeflection");
                break;
            case EnchantBracer_Stamina:
                handleEnchant(player, 852, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Stamina");
                break;
            case EnchantWeapon_LesserBeastslayer:
                handleEnchant(player, 853, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_LesserBeastslayer");
                break;
            case EnchantWeapon_LesserElementalSlayer:
                handleEnchant(player, 854, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_LesserElementalSlayer");
                break;
            case EnchantCloak_FireResistance:
                handleEnchant(player, 2463, COST_BRACKET_2, EQUIPMENT_SLOT_BACK, "EnchantCloak_FireResistance");
                break;
            case EnchantShield_Spirit:
                handleEnchant(player, 851, COST_BRACKET_2, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_Spirit");
                break;
            case EnchantBracer_Strength:
                handleEnchant(player, 856, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Strength");
                break;
            case EnchantChest_GreaterMana:
                handleEnchant(player, 857, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_GreaterMana");
                break;
            case EnchantBoots_LesserSpirit:
                handleEnchant(player, 255, COST_BRACKET_2, EQUIPMENT_SLOT_FEET, "EnchantBoots_LesserSpirit");
                break;
            case EnchantWeapon_WintersMight:
                handleEnchant(player, 2443, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_WintersMight");
                break;
            case EnchantShield_LesserBlock:
                handleEnchant(player, 863, COST_BRACKET_2, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_LesserBlock");
                break;
            case EnchantWeapon_Striking:
                handleEnchant(player, 943, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Striking");
                break;
            case Enchant2HWeapon_Impact:
                handleEnchant(player, 1897, COST_BRACKET_2, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_Impact");
                break;
            case EnchantGloves_Skinning:
                handleEnchant(player, 865, COST_BRACKET_2, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Skinning");
                break;
            case EnchantChest_LesserStats:
                handleEnchant(player, 866, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_LesserStats");
                break;
            case EnchantCloak_GreaterDefense:
                handleEnchant(player, 884, COST_BRACKET_2, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterDefense");
                break;
            case EnchantCloak_Resistance:
                handleEnchant(player, 903, COST_BRACKET_2, EQUIPMENT_SLOT_BACK, "EnchantCloak_Resistance");
                break;
            case EnchantGloves_Agility:
                handleEnchant(player, 904, COST_BRACKET_2, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Agility");
                break;
            case EnchantShield_Stamina:
                handleEnchant(player, 852, COST_BRACKET_2, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_Stamina");
                break;
            case EnchantBracer_Intellect:
                handleEnchant(player, 905, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Intellect");
                break;
            case EnchantBoots_Stamina:
                handleEnchant(player, 852, COST_BRACKET_2, EQUIPMENT_SLOT_FEET, "EnchantBoots_Stamina");
                break;
            case EnchantGloves_AdvancedMining:
                handleEnchant(player, 906, COST_BRACKET_2, EQUIPMENT_SLOT_HANDS, "EnchantGloves_AdvancedMining");
                break;
            case EnchantBracer_GreaterSpirit:
                handleEnchant(player, 907, COST_BRACKET_2, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_GreaterSpirit");
                break;
            case EnchantChest_SuperiorHealth:
                handleEnchant(player, 908, COST_BRACKET_2, EQUIPMENT_SLOT_CHEST, "EnchantChest_SuperiorHealth");
                break;
            case EnchantGloves_AdvancedHerbalism:
                handleEnchant(player, 909, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_AdvancedHerbalism");
                break;
            case EnchantCloak_LesserAgility:
                handleEnchant(player, 849, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_LesserAgility");
                break;
            case EnchantGloves_Strength:
                handleEnchant(player, 856, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Strength");
                break;
            case EnchantBoots_MinorSpeed:
                handleEnchant(player, 911, COST_BRACKET_3, EQUIPMENT_SLOT_FEET, "EnchantBoots_MinorSpeed");
                break;
            case EnchantShield_GreaterSpirit:
                handleEnchant(player, 907, COST_BRACKET_3, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_GreaterSpirit");
                break;
            case EnchantWeapon_Demonslaying:
                handleEnchant(player, 912, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Demonslaying");
                break;
            case EnchantChest_SuperiorMana:
                handleEnchant(player, 913, COST_BRACKET_3, EQUIPMENT_SLOT_CHEST, "EnchantChest_SuperiorMana");
                break;
            case EnchantBracer_Deflection:
                handleEnchant(player, 923, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_Deflection");
                break;
            case EnchantShield_FrostResistance:
                handleEnchant(player, 926, COST_BRACKET_3, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_FrostResistance");
                break;
            case EnchantBoots_Agility:
                handleEnchant(player, 904, COST_BRACKET_3, EQUIPMENT_SLOT_FEET, "EnchantBoots_Agility");
                break;
            case Enchant2HWeapon_GreaterImpact:
                handleEnchant(player, 963, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_GreaterImpact");
                break;
            case EnchantBracer_GreaterStrength:
                handleEnchant(player, 927, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_GreaterStrength");
                break;
            case EnchantChest_Stats:
                handleEnchant(player, 928, COST_BRACKET_3, EQUIPMENT_SLOT_CHEST, "EnchantChest_Stats");
                break;
            case EnchantWeapon_GreaterStriking:
                handleEnchant(player, 805, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_GreaterStriking");
                break;
            case EnchantBracer_GreaterStamina:
                handleEnchant(player, 929, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_GreaterStamina");
                break;
            case EnchantGloves_RidingSkill:
                handleEnchant(player, 930, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_RidingSkill");
                break;
            case EnchantGloves_MinorHaste:
                handleEnchant(player, 931, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_MinorHaste");
                break;
            case EnchantBracer_GreaterIntellect:
                handleEnchant(player, 1883, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_GreaterIntellect");
                break;
            case EnchantBoots_GreaterStamina:
                handleEnchant(player, 929, COST_BRACKET_3, EQUIPMENT_SLOT_FEET, "EnchantBoots_GreaterStamina");
                break;
            case EnchantWeapon_FieryWeapon:
                handleEnchant(player, 803, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_FieryWeapon");
                break;
            case EnchantCloak_GreaterResistance:
                handleEnchant(player, 1888, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterResistance");
                break;
            case EnchantShield_GreaterStamina:
                handleEnchant(player, 929, COST_BRACKET_3, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_GreaterStamina");
                break;
            case EnchantBracer_SuperiorSpirit:
                handleEnchant(player, 1884, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_SuperiorSpirit");
                break;
            case EnchantGloves_GreaterAgility:
                handleEnchant(player, 1887, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_GreaterAgility");
                break;
            case EnchantBoots_Spirit:
                handleEnchant(player, 851, COST_BRACKET_3, EQUIPMENT_SLOT_FEET, "EnchantBoots_Spirit");
                break;
            case EnchantChest_MajorHealth:
                handleEnchant(player, 1892, COST_BRACKET_3, EQUIPMENT_SLOT_CHEST, "EnchantChest_MajorHealth");
                break;
            case EnchantShield_SuperiorSpirit:
                handleEnchant(player, 1890, COST_BRACKET_3, EQUIPMENT_SLOT_OFFHAND, "EnchantShield_SuperiorSpirit");
                break;
            case EnchantCloak_SuperiorDefense:
                handleEnchant(player, 1889, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_SuperiorDefense");
                break;
            case EnchantWeapon_IcyChill:
                handleEnchant(player, 1894, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_IcyChill");
                break;
            case EnchantChest_MajorMana:
                handleEnchant(player, 1893, COST_BRACKET_3, EQUIPMENT_SLOT_CHEST, "EnchantChest_MajorMana");
                break;
            case EnchantWeapon_Strength:
                handleEnchant(player, 2563, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Strength");
                break;
            case EnchantWeapon_Agility:
                handleEnchant(player, 2564, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Agility");
                break;
            case EnchantBracer_ManaRegeneration:
                handleEnchant(player, 2565, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_ManaRegeneration");
                break;
            case Enchant2HWeapon_Agility:
                handleEnchant(player, 2646, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_Agility");
                break;
            case EnchantBracer_SuperiorStrength:
                handleEnchant(player, 1885, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_SuperiorStrength");
                break;
            case EnchantGloves_GreaterStrength:
                handleEnchant(player, 927, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_GreaterStrength");
                break;
            case EnchantBoots_GreaterAgility:
                handleEnchant(player, 1887, COST_BRACKET_3, EQUIPMENT_SLOT_FEET, "EnchantBoots_GreaterAgility");
                break;
            case Enchant2HWeapon_SuperiorImpact:
                handleEnchant(player, 1896, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_SuperiorImpact");
                break;
            case EnchantWeapon_UnholyWeapon:
                handleEnchant(player, 1899, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_UnholyWeapon");
                break;
            case EnchantBracer_SuperiorStamina:
                handleEnchant(player, 1886, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_SuperiorStamina");
                break;
            case EnchantChest_GreaterStats:
                handleEnchant(player, 1891, COST_BRACKET_3, EQUIPMENT_SLOT_CHEST, "EnchantChest_GreaterStats");
                break;
            case EnchantWeapon_SuperiorStriking:
                handleEnchant(player, 1897, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_SuperiorStriking");
                break;
            case EnchantWeapon_Lifestealing:
                handleEnchant(player, 1898, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Lifestealing");
                break;
            case EnchantWeapon_Crusader:
                handleEnchant(player, 1900, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_Crusader");
                break;
            case Enchant2HWeapon_MajorSpirit:
                handleEnchant(player, 1903, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_MajorSpirit");
                break;
            case Enchant2HWeapon_MajorIntellect:
                handleEnchant(player, 1904, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "Enchant2HWeapon_MajorIntellect");
                break;
            case EnchantWeapon_SpellPower:
                handleEnchant(player, 2504, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_SpellPower");
                break;
            case EnchantWeapon_HealingPower:
                handleEnchant(player, 2505, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_HealingPower");
                break;
            case EnchantBracer_HealingPower:
                handleEnchant(player, 2566, COST_BRACKET_3, EQUIPMENT_SLOT_WRISTS, "EnchantBracer_HealingPower");
                break;
            case EnchantWeapon_MightySpirit:
                handleEnchant(player, 2567, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MightySpirit");
                break;
            case EnchantWeapon_MightyIntellect:
                handleEnchant(player, 2568, COST_BRACKET_3, EQUIPMENT_SLOT_MAINHAND, "EnchantWeapon_MightyIntellect");
                break;
            case EnchantGloves_Threat:
                handleEnchant(player, 2613, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_Threat");
                break;
            case EnchantGloves_ShadowPower:
                handleEnchant(player, 2614, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_ShadowPower");
                break;
            case EnchantGloves_FrostPower:
                handleEnchant(player, 2615, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_FrostPower");
                break;
            case EnchantGloves_FirePower:
                handleEnchant(player, 2616, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_FirePower");
                break;
            case EnchantGloves_HealingPower:
                handleEnchant(player, 2617, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_HealingPower");
                break;
            case EnchantGloves_SuperiorAgility:
                handleEnchant(player, 2564, COST_BRACKET_3, EQUIPMENT_SLOT_HANDS, "EnchantGloves_SuperiorAgility");
                break;
            case EnchantCloak_GreaterFireResistance:
                handleEnchant(player, 2619, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterFireResistance");
                break;
            case EnchantCloak_GreaterNatureResistance:
                handleEnchant(player, 2620, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_GreaterNatureResistance");
                break;
            case EnchantCloak_Stealth:
                handleEnchant(player, 910, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_Stealth");
                break;
            case EnchantCloak_Subtlety:
                handleEnchant(player, 2621, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_Subtlety");
                break;
            case EnchantCloak_Dodge:
                handleEnchant(player, 2622, COST_BRACKET_3, EQUIPMENT_SLOT_BACK, "EnchantCloak_Dodge");
                break;
            }
        }
    }
    return true;
}

void AddSC_npc_enchanter()
{
    auto newscript = new Script;
    newscript->Name = "custom_enchant_npc";
    newscript->pGossipHello = &GossipHello_EnchantNPC;
    newscript->pGossipSelect = &GossipSelect_EnchantNPC;
    newscript->RegisterSelf(false);
}