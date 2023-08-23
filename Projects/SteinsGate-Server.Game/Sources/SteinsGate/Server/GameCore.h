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

struct GameServerProcessInfo;
struct RuntimeConfig;
struct GameContents;

NS_CORE_BEGIN
extern ::DataManager*			DataManager;
extern ::MysqlDatabase*			GameDB;
extern ::GameNetMaster*			NetMaster;
extern ::GameNetGroup*			NetGroup;
extern ::LogicServer*			Server;
extern ::GameServerType_t		GameServerType;
extern ::GameServerProcessInfo* GameServerProcessInfo;
extern ::RuntimeConfig*			RuntimeConfig;
extern ::GameContents			Contents;
NS_CORE_END

void InitializeGameCore(GameServerType_t gameServerType);
void FinalizeGameCore();