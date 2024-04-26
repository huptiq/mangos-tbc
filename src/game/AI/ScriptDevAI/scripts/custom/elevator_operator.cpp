#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"

bool GossipHello_elevator_operator(Player *player, Creature *creature)
{
    if (player->GetTeam() == HORDE)
    {
        if (player->GetPositionZ() < 0)
        {
            player->ADD_GOSSIP_ITEM(5, "Go up, please", GOSSIP_SENDER_MAIN, 1);
        }
        else
        {
            player->ADD_GOSSIP_ITEM(5, "Go down, please", GOSSIP_SENDER_MAIN, 2);
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_elevator_operator(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    switch (action)
    {
    case 1:
        player->GetPlayerMenu()->ClearMenus();
        player->TeleportTo(0, 1598.240845f, 240.227753f, 60.706398f, 6.280260f);
        break;
    case 2:
        player->GetPlayerMenu()->ClearMenus();
        player->TeleportTo(0, 1594.483398f, 311.006653f, -41.425461f, 3.138631f);
        break;
    }
    return true;
}

void AddSC_elevator_operator()
{
    auto newscript = new Script;
    newscript->Name = "custom_elevator_operator";
    newscript->pGossipHello = &GossipHello_elevator_operator;
    newscript->pGossipSelect = &GossipSelect_elevator_operator;
    newscript->RegisterSelf(false);
}