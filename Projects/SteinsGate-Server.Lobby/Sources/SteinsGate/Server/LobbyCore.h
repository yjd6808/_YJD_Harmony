/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ServerCore.h>

class DataManager;
class MysqlDatabase;
class LobbyNetMaster;
class LobbyNetGroup;
class LobbyServer;
class LobbyTokenManager;

extern DataManager* CoreDataManager_v;
extern MysqlDatabase* CoreGameDB_v;;
extern LobbyNetMaster* CoreNetMaster_v;
extern LobbyNetGroup* CoreNetGroup_v;
extern LobbyServer* CoreServer_v;
extern LobbyTokenManager* CoreTokenManager_v;

void InitializeLobbyCore();
void FinalizeLobbyCore();