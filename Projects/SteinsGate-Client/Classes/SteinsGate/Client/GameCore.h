/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 1:13:27 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonCore.h>

class SteinsGateApp;
class ClientInfo;
//class HostPlayer;
class DataManager;
//class UIManager;
//class Inven;
//class WorldScene;
//class FontManager;
//class ActorListenerManager;
//class ActorManager;
//class ImagePackManager;
//class Global;
//class PopupManager;
//class TimeManager;
class NetCore;

struct RuntimeConfig;
struct Contents;

NS_CORE_BEGIN
extern ::SteinsGateApp* App;
extern ::ClientInfo* ClientInfo;
extern ::Contents Contents;
// extern ::HostPlayer* Player;
extern ::DataManager* DataManager;
// extern ::UIManager* UIManager;
// extern ::Inven* Inven;
// extern ::WorldScene* World;
// extern ::FontManager* FontManager;
// extern ::ActorListenerManager* ActorListenerManager;
// extern ::ActorManager* ActorBox;
// extern ::ImagePackManager* PackManager;
// extern ::Global* Global;
// extern ::PopupManager* PopupManager;
// extern ::TimeManager* TimeManager;
extern ::NetCore* Net;
extern ::RuntimeConfig* RuntimeConfig;
NS_CORE_END

void InitializeClientCore();
void FinalizeClientCore();