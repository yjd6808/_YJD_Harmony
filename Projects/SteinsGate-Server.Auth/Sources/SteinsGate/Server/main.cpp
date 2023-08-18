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
	// ���� ���ҽ� �ʱ�ȭ
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
	InitializeAuthContents();

	// ======================================================
	// ���� ��ƾ
	// ======================================================

	if (CoreInterServerClientNetGroup_v && CoreInterServerClientNetGroup_v->ConnectCenterServer(5)) {
		CoreNetMaster_v->ProcessMainUpdate();
	}

	// ======================================================
	// ���� ���ҽ� ����
	// ======================================================

	FinalizeAuthContents();
	FinalizeAuthCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}
