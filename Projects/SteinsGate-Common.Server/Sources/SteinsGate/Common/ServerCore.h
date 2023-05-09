/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonCore.h>

class InputThread;
class CommonNetMaster;
class CommonNetGroup;
class CommonServer;

namespace JNetwork
{
	class TcpClient;
	class UdpClient;
}

extern InputThread* CoreInputThread_v;
extern CommonNetMaster* CoreCommonNetMaster_v;
extern CommonNetGroup* CoreCommonNetGroup_v;
extern CommonServer* CoreCommonServer_v;
extern JNetwork::TcpClient* CoreCenterClient_v;
extern JNetwork::UdpClient* CoreInterServerClient_v;

void InitializeServerCore();
void FinalizeServerCore();