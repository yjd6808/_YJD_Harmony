/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCore.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/Cmd.h>

USING_NS_JC;
USING_NS_JNET;

NS_CORE_BEGIN
::CLIThread*					CLIThread;
::ServerProcessInfoPackage*		ServerProcessInfoPackage;	// 메인 프로그램에서 주입해줄 것 (TODO: Common.Server로 옮기기)
::CommonInfo*					CommonInfo;					// 메인 프로그램에서 주입해줄 것	
::CharCommonInfo*				CharCommon;					// 메인 프로그램에서 주입해줄 것 (TODO: 전역으로 두기 좀 그럼)
::ThreadPool*					ThreadPool;					// 메인 프로그램에서 주입해줄 것
::Scheduler*					Scheduler;					// 메인 프로그램에서 주입해줄 것
::RuntimeConfigBase*			RuntimeConfigBase;			// 메인 프로그램에서 주입해줄 것
JNetwork::CommandNameDictionary CommandNameDictionary;
NS_CORE_END

void InitializeCommonCore() {
	DefaultParserType = PacketParser::Command;

	Core::CLIThread = dbg_new CLIThread();
	Core::CLIThread->Start();

	// 공통 커맨드 이름 등록
	// [ AUTHENTICATION ]
	Core::CommandNameDictionary.Add<CAU_Login>();
	Core::CommandNameDictionary.Add<AUC_LoginAck>();

	// [ LOBBY ]
	Core::CommandNameDictionary.Add<CLO_JoinLobby>();
	Core::CommandNameDictionary.Add<LOC_JoinLobbyAck>();

	// [ MESSAGE ]
	Core::CommandNameDictionary.Add<SC_ClientText>();
}

void FinalizeCommonCore() {
	if (Core::ThreadPool)
		Core::ThreadPool->Join(ThreadPool::JoinStrategy::WaitAllTasks);

	if (Core::Scheduler)
		Core::Scheduler->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

	Core::CLIThread->SendInterrupt();
	Core::CLIThread->Join();

	JCORE_DELETE_SAFE(Core::CLIThread);
	JCORE_DELETE_SAFE(Core::ThreadPool);
	JCORE_DELETE_SAFE(Core::Scheduler);
}