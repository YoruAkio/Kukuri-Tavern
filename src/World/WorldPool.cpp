﻿#include <World/WorldPool.hpp>
#include <Logger/Logger.hpp>
#include <Packet/VariantFunction.hpp>

WorldPool::~WorldPool() {
    m_worlds.clear();
}

std::shared_ptr<World> WorldPool::NewWorld(std::string vName) {
    if (m_worlds.find(vName) != m_worlds.end()) {
        Logger::Print(WARNING, "World {} already exists", vName);
        return nullptr;
    }

    auto world = std::make_shared<World>();
    m_worlds[vName] = world;
    Logger::Print(INFO, "Created new world: {}", vName);
    return world;
}

void WorldPool::RemoveWorld(std::string vName) {
    if (m_worlds.find(vName) == m_worlds.end()) {
        Logger::Print(WARNING, "World {} does not exist", vName);
        return;
    }
    m_worlds.erase(vName);
    Logger::Print(INFO, "Removed world: {}", vName);
}

std::shared_ptr<World> WorldPool::GetWorld(std::string vName) {
    if (m_worlds.find(vName) == m_worlds.end()) {
        return nullptr;
    }
    return m_worlds[vName];
}

void WorldPool::SendToWorldSelect(Player* pPlayer) {
    if (!pPlayer) return;

    // Welc msg
    VarList::OnConsoleMessage((ENetPeer*)pPlayer,
        fmt::format("Welcome {} Where would you like to go?", pPlayer->GetRawName()));

    // create World Select Menu with correct format
    std::string menuText = "default\n";
    menuText.append("add_filter|0|0|\n");
    menuText.append("\nadd_spacer|small|\n");
    menuText.append("add_label|big|`wTop Worlds``|left|\n");
    menuText.append("\nadd_spacer|small|\n");
    menuText.append("add_button|START||staticBlueFrame|0|\n");
    menuText.append("add_button|EXIT||staticBlueFrame|0|\n");

    // active worlds
    for (const auto& [name, world] : m_worlds) {
        if (world && world->GetPlayerCount() > 0) {
            menuText += fmt::format("add_button|{}||staticBlueFrame|{}|\n",
                name,
                world->GetPlayerCount()
            );
        }
    }

    // send World Select Menu
    auto vList = VariantList::Create("OnRequestWorldSelectMenu");
    vList.Insert(menuText);
    ENetWrapper::SendVariantList((ENetPeer*)pPlayer, vList);

    // Gazette dialog
    DialogBuilder db;
    db.SetDefaultColor('o')
        ->AddLabelWithIcon("`wThe Server Gazette``", 5016, LEFT, BIG)
        ->AddSpacer(SMALL)
        ->AddRawText("add_image_button||interface/large/news.rttex|bannerlayout|||\n")
        ->AddTextbox("Welcome to Kukuri Tavern")
        ->AddQuickExit()
        ->EndDialog("gazzette_end", "Cancel", "`wGet a GrowID``");

    auto dialogVList = VariantList::Create("OnDialogRequest", 100);
    dialogVList.Insert(db.Get());
    ENetWrapper::SendVariantList((ENetPeer*)pPlayer, dialogVList);

    Logger::Print(INFO, "Sent world select menu to player {}", pPlayer->GetRawName());
}

void WorldPool::JoinWorld(Player* pPlayer, const std::string& worldName) {
    if (!pPlayer) return;

    auto world = GetWorld(worldName);
    if (!world) {
        world = NewWorld(worldName);
        if (!world) {
            VarList::OnConsoleMessage((ENetPeer*)pPlayer,
                "`4Oops! `oUnable to create world!``");
            return;
        }
    }

    // TODO: spawn player in World
    // TODO: send World data to player

    Logger::Print(INFO, "Player {} joined world {}",
        pPlayer->GetRawName(), worldName);
}

std::unordered_map<std::string, std::shared_ptr<World>> WorldPool::GetWorlds() {
    return this->m_worlds;
}

size_t WorldPool::GetActiveWorlds() const {
    return m_worlds.size();
}