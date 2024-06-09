#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"
#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

uint32 bet_amount_slots;
std::string slot_1;
std::string slot_2;
std::string slot_3;

std::vector<std::string> slot_options = {
    "classicon_druid",
    "classicon_shaman",
    "classicon_warrior",
    "classicon_warlock",
    "classicon_priest",
    "classicon_mage",
    "classicon_rogue",
    "classicon_hunter",
    "classicon_paladin"};

void UpdateCasinoStats(int update)
{
    char query[256];
    sprintf(query, "SELECT jackpot FROM casino_stats LIMIT 1");

    std::unique_ptr<QueryResult> result(WorldDatabase.Query(query));
    if (result && result->GetRowCount() > 0)
    {
        int current_jackpot = result->Fetch()[0].GetInt32();
        int new_jackpot;
        if (update >= 0)
        {
            new_jackpot = current_jackpot + update;
        }
        else
        {
            new_jackpot = current_jackpot - (-update); // Taking the absolute value of negative update
        }
        sprintf(query, "UPDATE casino_stats SET jackpot = %d LIMIT 1", new_jackpot);
        WorldDatabase.DirectExecute(query);
    }
    else
    {
        if (update >= 0)
        {
            sprintf(query, "INSERT INTO casino_stats (jackpot) VALUES (%d)", update);
        }
        else
        {
            sprintf(query, "INSERT INTO casino_stats (jackpot) VALUES (%d)", -update); // Inserting positive value for negative update
        }
        WorldDatabase.DirectExecute(query);
    }
}

bool GossipHello_slots(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM(6, " Slot-Machine Payouts", GOSSIP_SENDER_MAIN, 21);
    if (player->GetMoney() >= 1000)
    {
        player->ADD_GOSSIP_ITEM(6, " Bet 10s", GOSSIP_SENDER_MAIN, 1);
    }
    if (player->GetMoney() >= 5000)
    {
        player->ADD_GOSSIP_ITEM(6, " Bet 50s", GOSSIP_SENDER_MAIN, 2);
    }
    if (player->GetMoney() >= 20000)
    {
        player->ADD_GOSSIP_ITEM(6, " Bet 2g", GOSSIP_SENDER_MAIN, 3);
    }
    if (player->GetMoney() >= 500000)
    {
        player->ADD_GOSSIP_ITEM(6, " Bet 50g", GOSSIP_SENDER_MAIN, 4);
    }
    if (player->GetMoney() >= 1000000)
    {
        player->ADD_GOSSIP_ITEM(6, " Bet 100g", GOSSIP_SENDER_MAIN, 5);
    }
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_slots(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    std::string suffix = "";
    switch (action)
    {
    case 1:
        bet_amount_slots = 1000;
        break;
    case 2:
        bet_amount_slots = 5000;
        break;
    case 3:
        bet_amount_slots = 20000;
        break;
    case 4:
        bet_amount_slots = 500000;
        break;
    case 5:
        bet_amount_slots = 1000000;
        break;
    case 6:
        GossipHello_slots(player, creature);
        break;
    case 21:
        player->GetPlayerMenu()->ClearMenus();
        player->ADD_GOSSIP_ITEM(6, " 2x ANY 1x", GOSSIP_SENDER_MAIN, 21);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_mage:30|t 3x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_rogue:30|t 3x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_hunter:30|t 3x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_paladin:30|t 3x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_shaman:30|t 10x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_warrior:30|t 10x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_warlock:30|t 10x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_priest:30|t 10x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 1x |TInterface\\icons\\classicon_druid:30|t 1.5x", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(6, " 3x |TInterface\\icons\\classicon_druid:30|t 20x", GOSSIP_SENDER_MAIN, 6);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
        break;
    }

    if (action < 6)
    {
        player->ModifyMoney(-bet_amount_slots);
        UpdateCasinoStats(bet_amount_slots);
        // Randomly select slot options
        uint32 payout_win;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, slot_options.size() - 1);

        // Spin the slots
        for (int i = 0; i < 3; ++i)
        {
            slot_1 = "|TInterface\\icons\\" + slot_options[dis(gen)] + ":60|t";
            slot_2 = "|TInterface\\icons\\" + slot_options[dis(gen)] + ":60|t";
            slot_3 = "|TInterface\\icons\\" + slot_options[dis(gen)] + ":60|t";

            // Update the player's view
            player->GetPlayerMenu()->ClearMenus();
            player->ADD_GOSSIP_ITEM(6, slot_1 + slot_2 + slot_3, GOSSIP_SENDER_MAIN, 20);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());

            // Wait for 1 second before showing next slot
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        // Check for winning combinations
        if (slot_1 == slot_2 && slot_2 == slot_3)
        {
            // All slots match
            uint32 payout = bet_amount_slots * 3; // Default payout multiplier
            if (slot_1 == "|TInterface\\icons\\classicon_druid:60|t")
            {
                creature->CastSpell(creature, 27745, TRIGGERED_OLD_TRIGGERED | TRIGGERED_DO_NOT_PROC, nullptr, nullptr, creature->GetObjectGuid());
                payout = bet_amount_slots * 20; // Druid icon
            }
            else if (slot_1 == "|TInterface\\icons\\classicon_shaman:60|t" ||
                     slot_1 == "|TInterface\\icons\\classicon_warrior:60|t" ||
                     slot_1 == "|TInterface\\icons\\classicon_warlock:60|t" ||
                     slot_1 == "|TInterface\\icons\\classicon_priest:60|t")
            {
                creature->CastSpell(creature, 12710, TRIGGERED_OLD_TRIGGERED | TRIGGERED_DO_NOT_PROC, nullptr, nullptr, creature->GetObjectGuid());
                payout = bet_amount_slots * 10; // Shaman, Warrior, Warlock, Priest icons
            }
            player->ModifyMoney(payout);
            UpdateCasinoStats(-payout);
            if (payout >= 10000)
            {
                payout_win = payout / 10000;
                suffix = "g!";
            }
            else
            {
                payout_win = payout / 100;
                suffix = "s!";
            }
            player->ADD_GOSSIP_ITEM(6, (" You won " + std::to_string(payout_win) + suffix).c_str(), GOSSIP_SENDER_MAIN, 20);
        }
        else if (slot_1 == "|TInterface\\icons\\classicon_druid:60|t" ||
                 slot_2 == "|TInterface\\icons\\classicon_druid:60|t" ||
                 slot_3 == "|TInterface\\icons\\classicon_druid:60|t")
        {
            // One slot is Druid
            uint32 payout = bet_amount_slots * 1.5;
            player->ModifyMoney(payout);
            UpdateCasinoStats(-payout);
            if (payout >= 10000)
            {
                payout_win = payout / 10000;
                suffix = "g!";
            }
            else
            {
                payout_win = payout / 100;
                suffix = "s!";
            }

            player->ADD_GOSSIP_ITEM(6, (" You won " + std::to_string(payout_win) + suffix).c_str(), GOSSIP_SENDER_MAIN, 20);
        }
        else if (slot_1 == slot_2 || slot_1 == slot_3 || slot_2 == slot_3)
        {
            // Two slots match
            uint32 payout = bet_amount_slots * 1;
            player->ModifyMoney(payout);
            UpdateCasinoStats(-payout);
            if (payout >= 10000)
            {
                payout_win = payout / 10000;
                suffix = "g!";
            }
            else
            {
                payout_win = payout / 100;
                suffix = "s!";
            }
            creature->CastSpell(creature, 12710, TRIGGERED_OLD_TRIGGERED | TRIGGERED_DO_NOT_PROC, nullptr, nullptr, creature->GetObjectGuid());
            player->ADD_GOSSIP_ITEM(6, (" You won " + std::to_string(payout_win) + suffix).c_str(), GOSSIP_SENDER_MAIN, 20);
        }
        else
        {
            // No matches
            player->ADD_GOSSIP_ITEM(6, " No win", GOSSIP_SENDER_MAIN, 20);
        }

        if (player->GetMoney() > bet_amount_slots)
        {
            if (bet_amount_slots == 1000)
            {
                player->ADD_GOSSIP_ITEM(5, " Spin again", GOSSIP_SENDER_MAIN, 1);
            }
            if (bet_amount_slots == 5000)
            {
                player->ADD_GOSSIP_ITEM(5, " Spin again", GOSSIP_SENDER_MAIN, 2);
            }
            if (bet_amount_slots == 20000)
            {
                player->ADD_GOSSIP_ITEM(5, " Spin again", GOSSIP_SENDER_MAIN, 3);
            }
            if (bet_amount_slots == 500000)
            {
                player->ADD_GOSSIP_ITEM(5, " Spin again", GOSSIP_SENDER_MAIN, 4);
            }
            if (bet_amount_slots == 1000000)
            {
                player->ADD_GOSSIP_ITEM(5, " Spin again", GOSSIP_SENDER_MAIN, 5);
            }
            player->ADD_GOSSIP_ITEM(5, " Change bet", GOSSIP_SENDER_MAIN, 6);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
    }

    return true;
}

void AddSC_slots()
{
    auto newscript = new Script;
    newscript->Name = "custom_slots";
    newscript->pGossipHello = &GossipHello_slots;
    newscript->pGossipSelect = &GossipSelect_slots;
    newscript->RegisterSelf(false);
}
