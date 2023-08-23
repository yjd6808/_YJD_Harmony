#include "Center.h"
#include "CenterCoreHeader.h"

#include <SteinsGate/Common/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int main() {
	new char;

	// ======================================================
	// 메인 리소스 초기화
	// ======================================================

	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(LOG_SPECIFIER_CENTER);
	InitializeDefaultLogger(LOG_SPECIFIER_CENTER);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerCenterLogo(true, 14);
	InitializeCenterCore();

	// ======================================================
	// 메인 루틴
	// ======================================================

	if (Core::NetGroup)
		Core::NetGroup->LaunchServer();

	if (Core::NetMaster)
		Core::NetMaster->ProcessMainUpdate();

	// ======================================================
	// 메인 리소스 정리
	// ======================================================

	FinalizeCenterCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}