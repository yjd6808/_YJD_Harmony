/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCore.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/AuthCmd.h>
#include <SteinsGate/Common/AreaCmd.h>
#include <SteinsGate/Common/ChatCmd.h>
#include <SteinsGate/Common/GameCmd.h>
#include <SteinsGate/Common/LobbyCmd.h>

USING_NS_JC;

ServerProcessInfoPackage* CoreServerProcessInfoPackage_v = nullptr;	// 메인 프로그램에서 주입해줄 것
CommonInfo* CoreCommonInfo_v = nullptr;					// 메인 프로그램에서 주입해줄 것
CharCommonInfo* CoreCharCommon_v = nullptr;				// 메인 프로그램에서 주입해줄 것
ThreadPool* CoreThreadPool_v = nullptr;					// 메인 프로그램에서 주입해줄 것
Scheduler* CoreScheduler_v = nullptr;					// 메인 프로그램에서 주입해줄 것
JNetwork::CommandNameDictionary CoreCommandNameDictionary_v;

void InitializeCommonCore() {

	// 공통 커맨드 이름 등록
	// [ AUTH ]
	CoreCommandNameDictionary_v.Add<CmdLogin>();
	CoreCommandNameDictionary_v.Add<CmdLoginAck>();
}

void FinalizeCommonCore() {
}