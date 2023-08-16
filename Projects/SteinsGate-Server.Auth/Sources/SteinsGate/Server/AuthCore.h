/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ServerCore.h>

class DataManager;
class MysqlDatabase;
class AuthNetMaster;
class AuthNetGroup;
class AuthServer;
class AuthenticationManager;
struct RuntimeConfig;

extern DataManager* CoreDataManager_v;
extern MysqlDatabase* CoreGameDB_v;;
extern AuthNetMaster* CoreNetMaster_v;
extern AuthNetGroup* CoreNetGroup_v;
extern AuthServer* CoreServer_v;
extern AuthenticationManager* CoreTokenManager_v;
extern RuntimeConfig* CoreRuntimeConfig_v;

void InitializeAuthCore();
void FinalizeAuthCore();