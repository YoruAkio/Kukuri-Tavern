#pragma once
#include <Event/EventType.hpp>
#include <Event/EventPool.hpp>
#include <Packet/TextFunction.hpp>
#include <Packet/VariantFunction.hpp>
#include <Packet/PacketFactory.hpp>
#include <Manager/Item/ItemManager.hpp>
#include <Utils/FileManager.hpp>

ACTION_EVENT("refresh_item_data", UpdatingItemsDat) {
    if (!pAvatar) return;

    // Send message that updating
    VarList::OnConsoleMessage((ENetPeer*)pAvatar, "`4Updating item data...``");

    auto packet = SUpdateItemsPacket(GetItemManager()->GetItemsData());
    ENetWrapper::SendPacket((ENetPeer*)pAvatar, packet);
    Logger::Print(DEBUG, "Updated items data for {}", pAvatar->GetRawName());
}