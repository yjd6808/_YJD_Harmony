/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <sgs/ServerCore.h>

class DataManager;
class MysqlDatabase;
class CenterNetMaster;
class CenterNetGroup;
class CenterServer;

struct RuntimeConfig;
struct CenterContents;

NS_SG_BEGIN
extern ::DataManager* DataManager;
extern ::MysqlDatabase* GameDB;
extern ::CenterNetMaster* NetGroupMgr;
extern ::CenterNetGroup* NetGroup;
extern ::CenterServer* Server;
extern ::RuntimeConfig* RuntimeConfig;
extern ::CenterContents		Contents;
NS_SG_END

void InitializeCenterCore();
void FinalizeCenterCore();