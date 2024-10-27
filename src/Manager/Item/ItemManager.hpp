#pragma once
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <Manager/Item/ItemInfo.hpp>

class ItemManager {
public:
    bool Serialize();
    void Encode();
    void Kill();

    ItemInfo* GetItem(uint32_t itemId);
    uint32_t GetItemsDatHash() const;

    std::vector<ItemInfo*> GetItems();
    std::vector<uint8_t> GetItemsData(); 
    size_t GetItemsLoaded();

public:
    ItemManager() : m_itemsDatHash(0) {} // constructor initialization
    ~ItemManager() = default;

private:
    uint16_t m_version;
    uint32_t m_itemCount;
    uint32_t m_itemsDatHash;
    std::vector<uint8_t> m_itemData;   

    std::vector<ItemInfo*> m_items;
};

ItemManager* GetItemManager();