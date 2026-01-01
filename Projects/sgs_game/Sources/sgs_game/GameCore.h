/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <sgs/ServerCore.h>
#include <sg/Enum.h>

class DataManager;
class MysqlDatabase;
class GameNetMaster;
class GameNetGroup;
class LogicServer;

struct GameServerProcessInfo;
struct RuntimeConfig;
struct GameContents;

NS_SG_BEGIN
extern ::DataManager*			DataManager;
extern ::MysqlDatabase*			GameDB;
extern ::GameNetMaster*			NetGroupMgr;
extern ::GameNetGroup*			NetGroup;
extern ::LogicServer*			Server;
extern ::GameServerType_t		GameServerType;
extern ::GameServerProcessInfo* GameServerProcessInfo;
extern ::RuntimeConfig*			RuntimeConfig;
extern ::GameContents			Contents;
NS_SG_END

void InitializeGameCore(GameServerType_t _gameServerType);
void FinalizeGameCore();
