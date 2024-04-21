#include "../../ScriptDevAIMgr.h"
#include "../../include/sc_gossip.h"
#include "../../../../World/World.h"
#include "../../../../Entities/Bag.h"
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
    player->ADD_GOSSIP_ITEM(10, "<Your choice will log you out>", GOSSIP_SENDER_MAIN, 1);

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
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_custom_xp(Player *player, Creature *creature, uint32 sender, uint32 action)
{
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
