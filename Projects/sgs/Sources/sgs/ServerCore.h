/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <sg/CommonCore.h>

class CommonNetGroupMgr;
class CommonNetGroup;
class CommonServer;
class InterServerClientNetGroup;
class TimeManager;

struct ServerProcessInfo;
struct RuntimeConfigCommon;
struct ServerContents;

NS_JNET_BEGIN
class TcpClient;
class UdpClient;
NS_JNET_END

NS_SG_BEGIN
extern ::CommonNetGroupMgr* CommonNetGroupMgr;
extern ::CommonNetGroup* CommonNetGroup;
extern ::CommonServer* CommonServer;
extern ::RuntimeConfigCommon* CommonRuntimeConfig;
extern ::ServerContents* CommonContents;
extern ::InterServerClientNetGroup* InterServerClientNetGroup;
extern ::jnet::TcpClient* InterServerClientTcp;
extern ::jnet::UdpClient* InterServerClientUdp;
extern ::ServerProcessInfo* ServerProcessInfo;
extern ::TimeManager* TimeManager;
NS_SG_END

void InitializeServerCore();
void FinalizeServerCore();
