#include "Lobby.h"
#include "LobbyCoreHeader.h"

#include <JCore/Random.h>

#include <SteinsGate/Server/LobbyContents.h>
#include <SteinsGate/Common/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int main() {
	new char; // 릭 확인용

	// ======================================================
	// 메인 리소스 초기화
	// ======================================================

	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(LOG_SPECIFIER_LOBBY);
	InitializeDefaultLogger(LOG_SPECIFIER_LOBBY);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerLobbyLogo(true, 24);
	InitializeLobbyCore();
	InitializeLobbyContents();

	// ======================================================
	// 메인 루틴
	// ======================================================

	if (CoreInterServerClientNetGroup_v && CoreInterServerClientNetGroup_v->ConnectCenterServer(5)) {
		CoreNetMaster_v->ProcessMainUpdate();
	}

	// ======================================================
	// 메인 리소스 정리
	// ======================================================

	FinalizeLobbyContents();
	FinalizeLobbyCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}
