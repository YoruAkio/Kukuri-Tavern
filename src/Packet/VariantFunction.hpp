#pragma once
#include <ENetWrapper/ENetWrapper.hpp>
#include <Packet/VariantList.hpp>

class VarList {
public:
    static VariantList OnSuperMainStartAcceptLogon(ENetPeer* peer, uint32_t itemsDatHash) {
        auto vList = VariantList::Create("OnSuperMainStartAcceptLogonHrdxs47254722215a");
        vList.Insert(itemsDatHash);  // items.dat hash
        vList.Insert("www.growtopia1.com");  // CDN host
        vList.Insert("growtopia/cache/");  // cache path
        vList.Insert("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster");
        vList.Insert("proto=210|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|server_tick=29671169|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|"); // metadata
        vList.Insert("2706858983"); // player hash
    
        ENetWrapper::SendVariantList(peer, vList);
        return vList;
    }
    static VariantList OnConsoleMessage(ENetPeer* peer, std::string message, int32_t delayMS = 0) {
        auto vList = VariantList::Create(__func__, delayMS);
        vList.Insert(message);
        
        ENetWrapper::SendVariantList(peer, vList);
        return vList;
    }
    static VariantList OnTextOverlay(ENetPeer* peer, std::string message, int32_t delayMS = 0) {
        auto vList = VariantList::Create(__func__, delayMS);
        vList.Insert(message);
        
        ENetWrapper::SendVariantList(peer, vList);
        return vList;
    }
    static VariantList SetHasGrowID(ENetPeer* peer, bool checkboxEnable, std::string tankIDName, std::string tankIDPass, int32_t delayMS = 0) {
        auto vList = VariantList::Create(__func__, delayMS);
        vList.Insert(checkboxEnable ? 1 : 0);
        vList.Insert(tankIDName);
        vList.Insert(tankIDPass);
        
        ENetWrapper::SendVariantList(peer, vList);
        return vList;
    }
};
