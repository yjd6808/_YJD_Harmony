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

extern CommonNetMaster* CoreCommonNetMaster_v;
extern CommonNetGroup* CoreCommonNetGroup_v;
extern CommonServer* CoreCommonServer_v;
extern ServerProcessInfo* CoreServerProcessInfo_v;

extern InterServerClientNetGroup* CoreInterServerClientNetGroup_v;
extern JNetwork::TcpClient* CoreInterServerClientTcp_v;
extern JNetwork::UdpClient* CoreInterServerClientUdp_v;
extern TimeManager* CoreTimeManager_v;
extern RuntimeConfigCommon* CoreRuntimeConfigCommon_v;

void InitializeServerCore();
void FinalizeServerCore();