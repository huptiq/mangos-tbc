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
            player->TeleportTo(0, 9003.46f, 870.031f, 29.6206f, 5.28f);
            break;
        case 3: // ironforge
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(0, -4613.62f, -915.38f, 501.062f, 3.88f);
            break;
        case 4: // darnassus
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(1, 9664.14f, 2526.36f, 1334.27f, 1.84454f);
            break;
        case 5: // orgrimmar
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(1, 1469.85f, -4221.52f, 58.9939f, 5.98f);
            break;
        case 6: // thunder bluff
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(1, -964.98f, 283.433f, 111.187f, 3.02f);
            break;
        case 7: // undercity
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(0, 1773.47f, 61.121f, -46.3207f, 0.54f);
            break;
        case 8: // shattrath WIP TODO
            player->ModifyMoney(-tp_cost);
            player->TeleportTo(0, 1773.47f, 61.121f, -46.3207f, 0.54f); // fix coords for shatt
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