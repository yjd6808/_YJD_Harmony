/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 1:13:27 AM
 * =====================
 *
 */


#pragma once

class SGClientInfo;
class SGHostPlayer;
class SGDataManager;
class SGUIManager;
class SGInven;
class SGWorldScene;
class SGFontPackage;
class SGActorListenerManager;
class SGActorBox;
class SGImagePackManager;
class SGGlobal;

extern SGClientInfo* CoreInfo_v;
extern SGHostPlayer* CorePlayer_v;
extern SGDataManager* CoreDataManager_v;
extern SGUIManager* CoreUIManager_v;
extern SGInven* CoreInven_v;
extern SGWorldScene* CoreWorld_v;
extern SGFontPackage* CoreFont_v;
extern SGActorListenerManager* CoreActorListenerManager_v;
extern SGActorBox* CoreActorBox_v;
extern SGImagePackManager* CorePackManager_v;
extern SGGlobal* CoreGlobal_v;

void InitializeClientCore();
void FinalizeClientCore();