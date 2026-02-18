/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <sgs/ServerCore.h>

NS_JDB_BEGIN
class MysqlDatabase;
NS_END

class DataManager;
class AuthNetMaster;
class AuthNetGroup;
class AuthServer;
class AuthenticationManager;
struct AuthContents;

NS_SG_BEGIN
extern ::DataManager*      DataManager;
extern ::jdb::MysqlDatabase* GameDB;
extern ::AuthNetMaster*    NetGroupMgr;
extern ::AuthNetGroup*     NetGroup;
extern ::AuthServer*       Server;
extern ::AuthContents      Contents;
NS_END

void InitializeAuthCore();
void FinalizeAuthCore();
