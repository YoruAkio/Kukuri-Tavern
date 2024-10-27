#pragma once
#include <Event/EventType.hpp>
#include <Event/EventPool.hpp>
#include <Packet/TextFunction.hpp>
#include <Packet/VariantFunction.hpp>

ACTION_EVENT("enter_game", OnEnterGame) {
    if (!pAvatar) return;

    VarList::OnConsoleMessage((ENetPeer*)pAvatar,
        fmt::format("`oWelcome to `wKukuri Tavern`o, {}!``", pAvatar->GetRawName()));

    // Create menu text
    std::string menuText = "default\n";
    menuText.append("add_filter|0|0|\n");
    menuText.append("\nadd_spacer|small|\n");
    menuText.append("add_label|big|`wTop Worlds``|left|\n");
    menuText.append("\nadd_spacer|small|\n");
    menuText.append("add_button|START||staticBlueFrame|0|\n");
    menuText.append("add_button|EXIT||staticBlueFrame|0|\n");

    // Create and send packet with extended data
    TankPacketData data{};
    data.m_type = NET_GAME_PACKET_CALL_FUNCTION;
    data.m_flags.bExtended = true;

    // Send as extended data
    ENetWrapper::SendPacket((ENetPeer*)pAvatar, NET_MESSAGE_GAME_PACKET, &data, sizeof(TankPacketData) + menuText.length());

    Logger::Print(DEBUG, "Player {} entered game", pAvatar->GetRawName());
}