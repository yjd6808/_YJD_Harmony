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

	if (CoreServer_v)
		CoreServer_v->Start(CoreServerProcessInfoPackage_v->Center.BindInterServerTcp);

	if (CoreNetMaster_v)
		CoreNetMaster_v->ProcessMainUpdate();

	// ======================================================
	// 메인 리소스 정리
	// ======================================================

	FinalizeCenterCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}