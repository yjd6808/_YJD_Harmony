/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbyNetMaster.h"

#include <SteinsGate/Common/ServerConst.h>

#include <SteinsGate/Server/LobbyNetGroup.h>
#include <SteinsGate/Server/LobbyInterServerClientNetGroup.h>



USING_NS_JC;
USING_NS_JNET;

LobbyNetMaster::LobbyNetMaster() : CommonNetMaster(LoopPerSecondLobby_v) {}

LobbyNetMaster::~LobbyNetMaster() {
}

void LobbyNetMaster::Initialize() {
	CommonNetMaster::Initialize();

	const auto spLobbyNetGroup = MakeShared<LobbyNetGroup>();
	const auto spInterServerNetGroup = MakeShared<LobbyInterServerClientNetGroup>();

	AddNetGroup(NETGROUP_ID_MAIN,			spLobbyNetGroup);
	AddNetGroup(NETGROUP_ID_INTERSERVER,	spInterServerNetGroup);

	spLobbyNetGroup->Initialize();
	spInterServerNetGroup->Initialize();
}

void LobbyNetMaster::OnUpdate(const TimeSpan& elapsed) {

#if DebugMode
	static TimeCounter s;
	s.Elapsed += elapsed;

	Thread::Sleep(30);

	if (s.ElapsedSeconds(5)) {
		//_LogDebug_("중앙 시각 %s", CoreTimeManager_v->Now().FormatMysqlTime().Source());
		//_LogDebug_("현재 시각 %s", DateTime::Now().FormatMysqlTime().Source());
	}
#endif

}

void LobbyNetMaster::OnStopped() {
}


