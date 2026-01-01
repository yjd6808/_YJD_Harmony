/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "GameNetMaster.h"

#include <sgs/ServerConst.h>

#include <sgs_game/GameNetGroup.h>
#include <sgs_game/GameInterServerClientNetGroup.h>


USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
GameNetMaster::GameNetMaster()
	: CommonNetGroupMgr(Const::Process::LoopPerSecondGame)
{
	SetName("게임");
}

//////////////////////////////////////////////////////////////////////////////////////////
GameNetMaster::~GameNetMaster()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameNetMaster::Initialize()
{
	CommonNetGroupMgr::Initialize();

	const auto gameNetGroup = MakeShared<GameNetGroup>();
	const auto interServerNetGroup = MakeShared<GameInterServerClientNetGroup>();

	AddNetGroup(Const::NetGroup::MainId, gameNetGroup);
	AddNetGroup(Const::NetGroup::InterServerId, interServerNetGroup);

	gameNetGroup->Initialize();
	interServerNetGroup->Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameNetMaster::OnUpdate(const TimeSpan& _elapsed)
{
#if _DEBUG
	static TimeCounter s;
	s.Elapsed += _elapsed;

	Thread::Sleep(30);

	if (s.ElapsedSeconds(5))
	{
		//_LogDebug_("중앙 시각 %s", sg::Contents.TimeManager->Now().FormatMysqlTime().Source());
		//_LogDebug_("현재 시각 %s", DateTime::Now().FormatMysqlTime().Source());
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameNetMaster::OnStopped()
{
}

