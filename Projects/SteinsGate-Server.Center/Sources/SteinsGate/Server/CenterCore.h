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

extern DataManager* CoreDataManager_v;
extern MysqlDatabase* CoreGameDB_v;;
extern CenterNetMaster* CoreNetMaster_v;

void InitializeCenterCore();
void FinalizeCenterCore();