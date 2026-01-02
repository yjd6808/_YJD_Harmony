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
class AuthNetMaster;
class AuthNetGroup;
class AuthServer;
class AuthenticationManager;

struct RuntimeConfig;
struct AuthContents;

NS_SG_BEGIN
extern ::DataManager*      DataManager;
extern ::MysqlDatabase*    GameDB;
extern ::AuthNetMaster*    NetGroupMgr;
extern ::AuthNetGroup*     NetGroup;
extern ::AuthServer*       Server;
extern ::RuntimeConfig*    RuntimeConfig;
extern ::AuthContents      Contents;
NS_SG_END

void InitializeAuthCore();
void FinalizeAuthCore();
