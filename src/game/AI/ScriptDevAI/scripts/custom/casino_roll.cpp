#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"
#include "./casino_slots.cpp"
#include <string>
#include <thread>
#include <iostream>
#include <random>

uint32 bet_amount_roll;
uint32 bet_flip;
uint32 flip_result;
uint32 amount_won;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> distribution(1, 2);
int random_value = distribution(gen);

bool GossipHello_CasinoRoll(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM(6, " Flip-a-Coin pays 2:1", GOSSIP_SENDER_MAIN, 30);
    player->ADD_GOSSIP_ITEM(5, " Bet 50s", GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM(5, " Bet 1g", GOSSIP_SENDER_MAIN, 2);
    player->ADD_GOSSIP_ITEM(5, " Bet 10g", GOSSIP_SENDER_MAIN, 3);
    player->ADD_GOSSIP_ITEM(5, " Bet 50g", GOSSIP_SENDER_MAIN, 4);
    player->ADD_GOSSIP_ITEM(5, " Bet 100g", GOSSIP_SENDER_MAIN, 5);
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}
bool GossipSelect_CasinoRoll(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if (action < 20)
    {
        switch (action)
        {
        case 1:
            if (player->GetMoney() > 5000)
            {
                bet_amount_roll = 5000;
                player->ModifyMoney(-bet_amount_roll);
                UpdateCasinoStats(bet_amount_roll);
                player->ADD_GOSSIP_ITEM(5, " Heads", GOSSIP_SENDER_MAIN, 20);
                player->ADD_GOSSIP_ITEM(5, " Tails", GOSSIP_SENDER_MAIN, 21);
            }
            else
            {
                player->GetSession()->SendNotification("You don't have enough money to place that bet");
                player->CLOSE_GOSSIP_MENU();
            }
            break;
        case 2:
            if (player->GetMoney() > 10000)
            {
                bet_amount_roll = 10000;
                player->ModifyMoney(-bet_amount_roll);
                UpdateCasinoStats(bet_amount_roll);
                player->ADD_GOSSIP_ITEM(5, " Heads", GOSSIP_SENDER_MAIN, 20);
                player->ADD_GOSSIP_ITEM(5, " Tails", GOSSIP_SENDER_MAIN, 21);
            }
            else
            {
                player->GetSession()->SendNotification("You don't have enough money to place that bet");
                player->CLOSE_GOSSIP_MENU();
            }
            break;
        case 3:
            if (player->GetMoney() > 100000)
            {
                bet_amount_roll = 100000;
                player->ModifyMoney(-bet_amount_roll);
                UpdateCasinoStats(bet_amount_roll);
                player->ADD_GOSSIP_ITEM(5, " Heads", GOSSIP_SENDER_MAIN, 20);
                player->ADD_GOSSIP_ITEM(5, " Tails", GOSSIP_SENDER_MAIN, 21);
            }
            else
            {
                player->GetSession()->SendNotification("You don't have enough money to place that bet");
                player->CLOSE_GOSSIP_MENU();
            }
            break;
        case 4:
            if (player->GetMoney() > 500000)
            {
                bet_amount_roll = 500000;
                player->ModifyMoney(-bet_amount_roll);
                UpdateCasinoStats(bet_amount_roll);
                player->ADD_GOSSIP_ITEM(5, " Heads", GOSSIP_SENDER_MAIN, 20);
                player->ADD_GOSSIP_ITEM(5, " Tails", GOSSIP_SENDER_MAIN, 21);
            }
            else
            {
                player->GetSession()->SendNotification("You don't have enough money to place that bet");
                player->CLOSE_GOSSIP_MENU();
            }
            break;
        case 5:
            if (player->GetMoney() > 1000000)
            {
                bet_amount_roll = 1000000;
                player->ModifyMoney(-bet_amount_roll);
                UpdateCasinoStats(bet_amount_roll);
                player->ADD_GOSSIP_ITEM(5, " Heads", GOSSIP_SENDER_MAIN, 20);
                player->ADD_GOSSIP_ITEM(5, " Tails", GOSSIP_SENDER_MAIN, 21);
            }
            else
            {
                player->GetSession()->SendNotification("You don't have enough money to place that bet");
                player->CLOSE_GOSSIP_MENU();
            }
            break;
        }
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
    }
    else
    {
        uint32 bet_flip = 0;
        uint32 flip_result = 0;
        uint32 amount_won = 0;
        std::string suffix = "";
        switch (action)
        {
        case 20:
            bet_flip = 1;
            player->ADD_GOSSIP_ITEM(6, " The Dealer flips the coin", GOSSIP_SENDER_MAIN, 30);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
            sleep(1);
            flip_result = distribution(gen);
            if (bet_amount_roll >= 10000)
            {
                amount_won = bet_amount_roll / 10000;
                suffix = "g!";
            }
            else
            {
                amount_won = bet_amount_roll / 100;
                suffix = "s!";
            }
            if (bet_flip == flip_result)
            {
                player->ADD_GOSSIP_ITEM(6, " Heads wins!", GOSSIP_SENDER_MAIN, 30);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
                sleep(1);
                player->ADD_GOSSIP_ITEM(6, (" You won " + std::to_string(amount_won) + suffix).c_str(), GOSSIP_SENDER_MAIN, 30);
                if (bet_amount_roll == 100000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 3);
                }
                else if (bet_amount_roll == 10000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 2);
                }
                else
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 1);
                }
                player->ModifyMoney(2 * bet_amount_roll);
                UpdateCasinoStats(-(2 * bet_amount_roll));
                creature->HandleEmote(18);
                player->HandleEmote(4);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
            }
            else
            {
                player->ADD_GOSSIP_ITEM(6, " Tails wins!", GOSSIP_SENDER_MAIN, 30);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
                sleep(1);
                player->ADD_GOSSIP_ITEM(6, " You lost the bet", GOSSIP_SENDER_MAIN, 30);
                if (bet_amount_roll == 100000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 3);
                }
                else if (bet_amount_roll == 10000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 2);
                }
                else
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 1);
                }
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
                player->HandleEmote(18);
            }
            break;
        case 21:
            bet_flip = 1;
            player->ADD_GOSSIP_ITEM(6, " The Dealer flips the coin", GOSSIP_SENDER_MAIN, 30);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
            sleep(1);
            flip_result = distribution(gen);
            if (bet_flip == flip_result)
            {
                player->ADD_GOSSIP_ITEM(6, " Tails wins!", GOSSIP_SENDER_MAIN, 30);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
                sleep(1);
                player->ADD_GOSSIP_ITEM(6, (" You won " + std::to_string(amount_won) + suffix).c_str(), GOSSIP_SENDER_MAIN, 30);
                if (bet_amount_roll == 100000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 3);
                }
                else if (bet_amount_roll == 10000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 2);
                }
                else
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 1);
                }
                player->ModifyMoney(2 * bet_amount_roll);
                UpdateCasinoStats(-(2 * bet_amount_roll));
                creature->HandleEmote(18);
                player->HandleEmote(4);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
            }
            else
            {
                player->ADD_GOSSIP_ITEM(6, " Heads wins!", GOSSIP_SENDER_MAIN, 30);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
                sleep(1);
                player->ADD_GOSSIP_ITEM(6, " You lost the bet", GOSSIP_SENDER_MAIN, 30);
                if (bet_amount_roll == 100000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 3);
                }
                else if (bet_amount_roll == 10000)
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 2);
                }
                else
                {
                    player->ADD_GOSSIP_ITEM(5, " Repeat the bet", GOSSIP_SENDER_MAIN, 1);
                }
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetObjectGuid());
                player->HandleEmote(18);
            }
            break;
        }
    }
    return true;
}

void AddSC_casino_roll()
{
    auto newscript = new Script;
    newscript->Name = "custom_casino_roll";
    newscript->pGossipHello = &GossipHello_CasinoRoll;
    newscript->pGossipSelect = &GossipSelect_CasinoRoll;
    newscript->RegisterSelf(false);
}