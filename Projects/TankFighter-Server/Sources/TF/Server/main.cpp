/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 7:23:12 PM
 * =====================
 *
 */

#include "Pch.h"

#include <TF/Server/NetCore.h>
#include <TF/Common/Command.h>

USING_NS_JC;
USING_NS_JNET;

int main() {
	new char; // Force Leak
	//_CrtSetBreakAlloc(1556);
	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	InitializeDefaultLogger();
	InitializeNetLogger();
	InitializeCommon();
	InitializeCore();

	Console::WriteLine("X키 입력시 종료");

	if (Core::NetCore)
		Core::NetCore->ProcessMainLoop();

	FinalizeCore();
	FinalizeCommon();
	FinalizeNetLogger();
	FinalizeDefaultLogger();
	Winsock::Finalize();
	return 0;
}
