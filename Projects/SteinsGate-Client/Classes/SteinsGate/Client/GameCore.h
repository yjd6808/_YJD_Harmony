/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 1:13:27 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonCore.h>

class ClientInfo;
class HostPlayer;
class DataManager;
class UIManager;
class Inven;
class WorldScene;
class FontPackage;
class ActorListenerManager;
class ActorBox;
class ImagePackManager;
class Global;

extern ClientInfo* CoreClient_v;
extern HostPlayer* CorePlayer_v;
extern DataManager* CoreDataManager_v;
extern UIManager* CoreUIManager_v;
extern Inven* CoreInven_v;
extern WorldScene* CoreWorld_v;
extern FontPackage* CoreFont_v;
extern ActorListenerManager* CoreActorListenerManager_v;
extern ActorBox* CoreActorBox_v;
extern ImagePackManager* CorePackManager_v;
extern Global* CoreGlobal_v;

void InitializeClientCore();
void FinalizeClientCore();