#include "Auth.h"
#include "AuthCoreHeader.h"

#include <JCore/Random.h>

#include <SteinsGate/Server/AuthContents.h>
#include <SteinsGate/Common/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int main() {
	new char;

	// ======================================================
	// 메인 리소스 초기화
	// ======================================================

	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(LOG_SPECIFIER_AUTH);
	InitializeDefaultLogger(LOG_SPECIFIER_AUTH);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerAuthLogo(true, 24);
	InitializeAuthCore();

	// ======================================================
	// 메인 루틴
	// ======================================================

	

	if (Core::InterServerClientNetGroup && Core::InterServerClientNetGroup->ConnectCenterServer(5)) {
		Core::NetMaster->ProcessMainUpdate();
	}

	// ======================================================
	// 메인 리소스 정리
	// ======================================================

	FinalizeAuthCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);
}
