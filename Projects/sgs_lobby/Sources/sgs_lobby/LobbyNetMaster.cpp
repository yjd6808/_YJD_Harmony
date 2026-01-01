/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "LobbyCoreHeader.h"
#include "LobbyNetMaster.h"

#include <sgs/ServerConst.h>

#include <sgs_lobby/LobbyNetGroup.h>
#include <sgs_lobby/LobbyInterServerClientNetGroup.h>


USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
LobbyNetMaster::LobbyNetMaster()
	: CommonNetGroupMgr(Const::Process::LoopPerSecondLobby)
{
	SetName("로비");
}

//////////////////////////////////////////////////////////////////////////////////////////
LobbyNetMaster::~LobbyNetMaster()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyNetMaster::Initialize()
{
	CommonNetGroupMgr::Initialize();

	const auto lobbyNetGroup = MakeShared<LobbyNetGroup>();
	const auto interServerNetGroup = MakeShared<LobbyInterServerClientNetGroup>();

	AddNetGroup(Const::NetGroup::MainId, lobbyNetGroup);
	AddNetGroup(Const::NetGroup::InterServerId, interServerNetGroup);

	lobbyNetGroup->Initialize();
	interServerNetGroup->Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyNetMaster::OnUpdate(const TimeSpan& _elapsed)
{
#if _DEBUG
	static TimeCounter timeCounter;
	timeCounter.Elapsed += _elapsed;

	Thread::Sleep(30);

	if (timeCounter.ElapsedSeconds(5))
	{
		//_LogDebug_("중앙 시각 %s", sg::Contents.TimeManager->Now().FormatMysqlTime().Source());
		//_LogDebug_("현재 시각 %s", DateTime::Now().FormatMysqlTime().Source());
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyNetMaster::OnStopped()
{
}

