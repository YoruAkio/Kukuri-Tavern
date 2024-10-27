#pragma once
#if defined(_WIN32) || defined(WIN32)
#pragma comment(lib, "winmm.lib")
#endif

#include <thread>
#include <string>
#include <enet/enet.h>
#include <fmt/core.h>
#include <ENetWrapper/Peer.hpp>
#include <ENetWrapper/ENetServer.hpp>
#include <Packet/PacketFactory.hpp>
#include <Packet/VariantList.hpp>
#include <Player/Player.hpp>
#include <Logger/Logger.hpp>

namespace ENetWrapper {  
    inline void SendPacket(ENetPeer* peer, ISPacket& data) {
        if (!peer) 
            return;
        auto packetData = data.m_packetData.data();
        auto dataLength = data.m_packetLength;

        ENetPacket* packet = enet_packet_create(nullptr, 5 + dataLength, ENET_PACKET_FLAG_RELIABLE);
        if (!packet)
            return;

        std::memcpy(packet->data, &data.m_packetType, 4);
        packet->data[dataLength + 4] = 0;

        if ((data.m_packetType != NET_MESSAGE_SERVER_HELLO && dataLength > sizeof(TankPacketData)) ||
            (data.m_packetType == NET_MESSAGE_GAME_MESSAGE))
            std::memcpy(packet->data + 4, packetData, dataLength);

        if (enet_peer_send(peer, 0, packet) != 0)
            enet_packet_destroy(packet);
    }
    inline void SendPacket(ENetPeer* peer, eNetMessageType type, const void* data, size_t length) {
        if (!peer) return;

        ENetPacket* packet = enet_packet_create(nullptr, length + 5, ENET_PACKET_FLAG_RELIABLE);
        if (!packet) return;

        std::memcpy(packet->data, &type, 4);
        if (data && length > 0) {
            std::memcpy(packet->data + 4, data, length);
        }
        packet->data[length + 4] = 0;

        if (enet_peer_send(peer, 0, packet) != 0) {
            enet_packet_destroy(packet);
            Logger::Print(WARNING, "Failed to send packet type {} (length: {})",
                static_cast<int>(type),
                static_cast<int>(length)
            );
            return;
        }

        Logger::Print(DEBUG, "Successfully sent packet - Type: {}, Length: {}",
            static_cast<int>(type),
            static_cast<int>(length)
        );
    }
    /*inline void SendPacket(ENetPeer* peer, eNetMessageType packetType, const void* pData, uintmax_t dataLength) {
        if (!peer) 
            return;

        ENetPacket* packet = enet_packet_create(nullptr, 5 + dataLength, ENET_PACKET_FLAG_RELIABLE);
        if (!packet)
            return;
            
        std::memcpy(packet->data, &packetType, 4);
        packet->data[dataLength + 4] = 0;
        
        if (pData)
            std::memcpy(packet->data + 4, pData, dataLength);

        if (enet_peer_send(peer, 0, packet) != 0)
            enet_packet_destroy(packet);
    }*/

    inline void SendPacket(ENetPeer* peer, std::string data) {
        auto vPacket = STextPacket(data);
        ENetWrapper::SendPacket(peer, vPacket);
    }

    inline void SendVariantList(ENetPeer* peer, VariantList vList) {
        auto vPacket = SVariantPacket(vList);
        ENetWrapper::SendPacket(peer, vPacket);

        Logger::Print(DEBUG, "Sent variant list packet with {} variants",
            static_cast<int>(vList.GetTotalObjects())
        );
    }
}