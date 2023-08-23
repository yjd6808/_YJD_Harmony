/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonCore.h>

class CommonNetMaster;
class CommonNetGroup;
class CommonServer;
struct ServerProcessInfo;
class InterServerClientNetGroup;
class TimeManager;
struct RuntimeConfigCommon;

NS_JNET_BEGIN
class TcpClient;
class UdpClient;
NS_JNET_END

NS_CORE_BEGIN
extern ::CommonNetMaster* CommonNetMaster;
extern ::CommonNetGroup* CommonNetGroup;
extern ::CommonServer* CommonServer;
extern ::ServerProcessInfo* ServerProcessInfo;

extern ::InterServerClientNetGroup* InterServerClientNetGroup;
extern ::JNetwork::TcpClient* InterServerClientTcp;
extern ::JNetwork::UdpClient* InterServerClientUdp;
extern ::TimeManager* TimeManager;
extern ::RuntimeConfigCommon* RuntimeConfigCommon;
NS_CORE_END

void InitializeServerCore();
void FinalizeServerCore();