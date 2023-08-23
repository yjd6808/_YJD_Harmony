/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCore.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/Cmd_AUTH.h>
#include <SteinsGate/Common/Cmd_CHAT.h>

USING_NS_JC;

NS_CORE_BEGIN
	::CLIThread* CLIThread;
	::ServerProcessInfoPackage* ServerProcessInfoPackage = nullptr;	// 메인 프로그램에서 주입해줄 것
	::CommonInfo* CommonInfo = nullptr;								// 메인 프로그램에서 주입해줄 것
	::CharCommonInfo* CharCommon = nullptr;							// 메인 프로그램에서 주입해줄 것
	::ThreadPool* ThreadPool = nullptr;								// 메인 프로그램에서 주입해줄 것
	::Scheduler* Scheduler = nullptr;								// 메인 프로그램에서 주입해줄 것
	::RuntimeConfigBase* RuntimeConfigBase = nullptr;				// 메인 프로그램에서 주입해줄 것
	JNetwork::CommandNameDictionary CommandNameDictionary;
NS_CORE_END

void InitializeCommonCore() {
	Core::CLIThread = dbg_new CLIThread();
	Core::CLIThread->Start();

	// 공통 커맨드 이름 등록
	// [ AUTH ]
	Core::CommandNameDictionary.Add<CAU_Login>();
	Core::CommandNameDictionary.Add<AUC_LoginAck>();
}

void FinalizeCommonCore() {
	Core::CLIThread->SendInterrupt();
	Core::CLIThread->Join();
	JCORE_DELETE_SAFE(Core::CLIThread);
}