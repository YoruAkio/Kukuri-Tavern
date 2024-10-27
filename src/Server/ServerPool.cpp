#include <Server/ServerPool.hpp>
#include <magic_enum.hpp>
#include <ENetWrapper/ENetServer.hpp>
#include <config.hpp>
#include <Logger/Logger.hpp>
#include <Event/EventPool.hpp>
#include <Packet/PacketFactory.hpp>
#include <Utils/Utils.hpp>
#include <Packet/VariantFunction.hpp>
#include <Manager/Item/ItemManager.hpp>
#include <fmt/format.h>

ServerPool g_serverPool;
ServerPool* GetServerPool() {
    return &g_serverPool;
}

void ServerPool::Start() {
	std::thread t(&ServerPool::ServicePoll, this);
	this->m_serviceThread = std::move(t);
    this->running.store(true);
}
void ServerPool::Stop() {
    this->running.store(false);
}
bool ServerPool::IsRunning() const {
    return this->running.load();
}

std::shared_ptr<Server> ServerPool::StartInstance() {
    uint8_t instanceId = static_cast<uint8_t>(m_servers.size());
    auto tempPort = Configuration::GetBasePort() + this->serverOffset++;   
    auto server{ std::make_shared<Server>() };

    if (!server->Init(Configuration::GetBaseHost(), tempPort, 0xFF)) {
        Logger::Print(EXCEPTION, "Failed to Initialize {}, port {} is busy...", fmt::format(fmt::emphasis::bold | fg(fmt::color::cornsilk), "ENetServer"), fmt::format(fmt::emphasis::bold | fg(fmt::color::blue_violet), "{}", tempPort));
        return nullptr;
    }
    Logger::Print(INFO, "Starting up {} Instance", fmt::format(fmt::emphasis::bold | fg(fmt::color::cornsilk), "ENetServer"));
    Logger::Print("InstanceId: {}, Server located at {}", fmt::format(fmt::emphasis::bold | fg(fmt::color::lavender), "{}", instanceId), fmt::format(fmt::emphasis::bold | fg(fmt::color::cornsilk), "udp://{}:{}", Configuration::GetBaseHost(), tempPort));
    m_servers.insert_or_assign(instanceId, std::move(server));
    return m_servers[instanceId];
}
bool ServerPool::StopInstance(uint8_t instanceId) {
    if (this->m_servers.find(instanceId) == this->m_servers.end())
        return false;
    Logger::Print(INFO, "Shutting Down InstanceId {}", fmt::format(fmt::emphasis::bold | fg(fmt::color::cornsilk), "{}", instanceId));
    return this->m_servers.erase(instanceId) > 0;
}

std::unordered_map<uint8_t, std::shared_ptr<Server>> ServerPool::GetServers() {
    return this->m_servers;
}
std::shared_ptr<Server> ServerPool::GetBalancedServer() {
    for (auto [instanceId, server] : m_servers) {
        if (server->GetActivePlayers() > 1)
            continue;
        return server;
    }
    return this->StartInstance();
}

size_t ServerPool::GetActiveServers() const {
    return this->m_servers.size();
}
size_t ServerPool::GetActivePlayers() const {
    size_t ret{ 0 };
    for (auto [instanceId, server] : m_servers)
        ret += server->GetActivePlayers();
    return ret;
}
size_t ServerPool::GetActiveWorlds() const {
    size_t ret{ 0 };
    for (auto [instanceId, server] : m_servers)
        ret += server->GetActiveWorlds();
    return ret;
}

void ServerPool::ServicePoll() {
    while (this->IsRunning()) {
        for (auto& [instanceId, pServer] : this->GetServers()) {
            try {
                while (enet_host_check_events(pServer->GetHost(), pServer->GetEvent())) {
                    auto event = *pServer->GetEvent();
                    auto playerPool = pServer->GetPlayerPool();

                    if (!event.peer || !playerPool) {
                        Logger::Print(WARNING, "Invalid peer or player pool");
                        break;
                    }

                    switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT: {
                        Logger::Print(INFO, "New connection attempt from {}",
                            static_cast<uint32_t>(event.peer->address.host));

                        try {
                            Player* pAvatar = playerPool->NewPlayer(event.peer);
                            if (pAvatar) {
                                pAvatar->OnConnect();
                                Logger::Print(INFO, "Player successfully connected");
                            }
                            else {
                                Logger::Print(WARNING, "Failed to create new player");
                            }
                        }
                        catch (const std::exception& e) {
                            Logger::Print(EXCEPTION, "Exception in connection handler: {}", e.what());
                        }
                    } break;

                    case ENET_EVENT_TYPE_RECEIVE: {
                        Player* pAvatar = playerPool->GetPlayer(event.peer->connectID);
                        if (!pAvatar) {
                            Logger::Print(WARNING, "Received packet from unknown player");
                            break;
                        }

                        if (event.packet->dataLength < sizeof(IPacketType::m_packetType) + 1 ||
                            event.packet->dataLength > 0x400) {
                            Logger::Print(WARNING, "Invalid packet size: {}", event.packet->dataLength);
                            enet_packet_destroy(event.packet);
                            break;
                        }

                        try {
                            auto messageType = *(int32_t*)event.packet->data;
                            Logger::Print(INFO, "Received packet type: {}", messageType);

                            switch (messageType) {
                            case NET_MESSAGE_GENERIC_TEXT:
                            case NET_MESSAGE_GAME_MESSAGE: {
                                auto data = this->DataToString(
                                    event.packet->data + sizeof(IPacketType::m_packetType),
                                    event.packet->dataLength - sizeof(IPacketType::m_packetType)
                                );
                                Logger::Print(DEBUG, "Got message: {}", data);

                                if (data.find("protocol|") != std::string::npos) {
                                    Logger::Print(DEBUG, "Received protocol message from guest");

                                    // Send OnSuperMain
                                    VarList::OnSuperMainStartAcceptLogon(event.peer, GetItemManager()->GetItemsDatHash());
                                    Logger::Print(DEBUG, "Sent OnSuperMain");
                                }
                                else {
                                    GetEventPool()->AddQueue(
                                        data.substr(0, data.find('|')),
                                        pAvatar,
                                        pServer,
                                        data,
                                        TextParse(data),
                                        nullptr
                                    );
                                    Logger::Print(DEBUG, "Processed text message: {}", data);
                                }
                            } break;
                            case NET_MESSAGE_GAME_PACKET: {
                                Logger::Print(INFO, "Received packet type: Game Packet");
                                auto* tankPacket = this->DataToTankPacket(
                                    event.packet->data + sizeof(IPacketType::m_packetType),
                                    event.packet->dataLength - sizeof(IPacketType::m_packetType)
                                );
                                if (tankPacket) {
                                    Logger::Print(DEBUG, "Processing tank packet type: {}", static_cast<int>(tankPacket->m_type));
                                }
                            } break;
                            default:
                                Logger::Print(DEBUG, "Unknown message type: {}", messageType);
                                break;
                            }
                        }
                        catch (const std::exception& e) {
                            Logger::Print(DEBUG, "Exception in packet handler: {}", e.what());
                        }

                        if (event.packet) {
                            enet_packet_destroy(event.packet);
                        }
                    } break;

                    case ENET_EVENT_TYPE_DISCONNECT: {
                        try {
                            Player* pAvatar = playerPool->GetPlayer(event.peer->connectID);
                            if (pAvatar) {
                                pAvatar->OnDisconnect();
                                playerPool->RemovePlayer(event.peer->connectID);
                                Logger::Print(INFO, "Player disconnected: {}", event.peer->connectID);
                            }
                        }
                        catch (const std::exception& e) {
                            Logger::Print(EXCEPTION, "Exception in disconnect handler: {}", e.what());
                        }
                    } break;

                    default:
                        Logger::Print(WARNING, "Unknown event type: {}",
                            static_cast<int>(event.type));
                        break;
                    }
                }
                enet_host_service(pServer->GetHost(), nullptr, 1);
            }
            catch (const std::exception& e) {
                Logger::Print(EXCEPTION, "Exception in main server loop: {}", e.what());
            }
        }
    }
}