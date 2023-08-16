/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ServerCore.h>
#include <SteinsGate/Common/Enum.h>

class DataManager;
class MysqlDatabase;
class GameNetMaster;
class GameNetGroup;
class LogicServer;
class GameTokenManager;
struct GameServerProcessInfo;
struct RuntimeConfig;

extern DataManager* CoreDataManager_v;
extern MysqlDatabase* CoreGameDB_v;;
extern GameNetMaster* CoreNetMaster_v;
extern GameNetGroup* CoreNetGroup_v;
extern LogicServer* CoreServer_v;
extern GameTokenManager* CoreTokenManager_v;
extern GameServerType_t CoreGameServerType_v;
extern GameServerProcessInfo* CoreGameServerProcessInfo_v;
extern RuntimeConfig* CoreRuntimeConfig_v;

bool InitializeGameCore(GameServerType_t gameServerType);
void FinalizeGameCore();