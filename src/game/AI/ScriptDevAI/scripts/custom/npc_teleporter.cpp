#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"

uint32 tp_cost = 50000;

bool GossipHello_teleporter(Player *player, Creature *creature)
{
    if (player->GetMoney() >= tp_cost)
    {
        player->ADD_GOSSIP_ITEM(1, " Teleport to any city for just 5g", GOSSIP_SENDER_MAIN, 1);
        if (player->GetTeam() == ALLIANCE)
        {
            player->ADD_GOSSIP_ITEM(5, " Stormwind", GOSSIP_SENDER_MAIN, 2);
            player->ADD_GOSSIP_ITEM(5, " Ironforge", GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(5, " Darnassus", GOSSIP_SENDER_MAIN, 4);
        }
        else
        {
            player->ADD_GOSSIP_ITEM(5, " Orgrimmar", GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(5, " Thunder Bluff", GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(5, " Undercity", GOSSIP_SENDER_MAIN, 7);
        }
        if (player->GetLevel() > 59)
        {
            player->ADD_GOSSIP_ITEM(5, " Shattrath", GOSSIP_SENDER_MAIN, 8);
        }
    }
    else
    {
        player->ADD_GOSSIP_ITEM(1, " You don't have enough money to use this service", GOSSIP_SENDER_MAIN, 1);
    }
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_teleporter(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if (action > 1)
    {
        switch (action)
        {
        case 2: // stormwind
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(0, -5994.772461f, 857.292542f, 29.620701f, 2.241570f);
            break;
        case 3: // ironforge
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(0, -4613.397461f, -915.249634f, 501.062744f, 3.835466f);
            break;
        case 4: // darnassus
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(1, 9657.625977f, 2514.115967f, 1331.649780f, 1.163907f);
            break;
        case 5: // orgrimmar
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(1, 1470.439575f, -4220.841797f, 58.993835f, 6.175170f);
            break;
        case 6: // thunder bluff
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(1, -968.679810f, 284.176300f, 110.907990f, 2.575390f);
            break;
        case 7: // undercity
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(0, 1770.977539f, 63.246574f, -46.321075f, 2.343006f);
            break;
        case 8: // shattrath
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(530, -1810.907349f, 5309.564453f, -12.428000f, 1.863699f);
            break;
        }
    }
    return true;
}

void AddSC_teleporter()
{
    auto newscript = new Script;
    newscript->Name = "custom_teleporter";
    newscript->pGossipHello = &GossipHello_teleporter;
    newscript->pGossipSelect = &GossipSelect_teleporter;
    newscript->RegisterSelf(false);
}