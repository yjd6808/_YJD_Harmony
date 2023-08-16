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
class CenterNetMaster;
class CenterNetGroup;
class CenterServer;
struct RuntimeConfig;

extern DataManager* CoreDataManager_v;
extern MysqlDatabase* CoreGameDB_v;;
extern CenterNetMaster* CoreNetMaster_v;
extern CenterNetGroup* CoreNetGroup_v;
extern CenterServer* CoreServer_v;
extern RuntimeConfig* CoreRuntimeConfig_v;

void InitializeCenterCore();
void FinalizeCenterCore();