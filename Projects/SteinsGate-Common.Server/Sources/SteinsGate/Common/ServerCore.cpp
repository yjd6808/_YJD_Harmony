/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCore.h"
#include "ServerCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;

InputThread* CoreInputThread_v;
CommonNetMaster* CoreCommonNetMaster_v;				// 메인 서버 프로그램에서 주입해줄 것
CommonNetGroup* CoreCommonNetGroup_v;				// 메인 서버 프로그램에서 주입해줄 것
CommonServer* CoreCommonServer_v;					// 메인 서버 프로그램에서 주입해줄 것
TcpClient* CoreCenterClient_v;						// 메인 서버 프로그램에서 주입해줄 것
UdpClient* CoreInterServerClient_v;					// 메인 서버 프로그램에서 주입해줄 것

void InitializeServerCore() {
	CoreInputThread_v = dbg_new InputThread();
	CoreInputThread_v->Start();
}

void FinalizeServerCore() {
	CoreInputThread_v->Abort();		// std::cin에서 입력받는 상태를 강제로 부수기 위해서 어보트처리
	JCORE_DELETE_SAFE(CoreInputThread_v);
}