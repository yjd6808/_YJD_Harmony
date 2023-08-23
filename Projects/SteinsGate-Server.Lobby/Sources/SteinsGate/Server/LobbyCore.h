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

struct RuntimeConfig;
struct LobbyContents;

NS_CORE_BEGIN
extern ::DataManager*		DataManager;
extern ::MysqlDatabase*		GameDB;
extern ::LobbyNetMaster*	NetMaster;
extern ::LobbyNetGroup*		NetGroup;
extern ::LobbyServer*		Server;
extern ::RuntimeConfig*		RuntimeConfig;
extern ::LobbyContents		Contents;
NS_CORE_END

void InitializeLobbyCore();
void FinalizeLobbyCore();