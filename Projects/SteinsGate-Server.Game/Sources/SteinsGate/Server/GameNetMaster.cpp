﻿/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Game.h"
#include "GameCoreHeader.h"
#include "GameNetMaster.h"

#include <SteinsGate/Common/ServerConst.h>

#include <SteinsGate/Server/GameNetGroup.h>
#include <SteinsGate/Server/GameInterServerClientNetGroup.h>



USING_NS_JC;
USING_NS_JNET;

GameNetMaster::GameNetMaster() : CommonNetMaster(LoopPerSecondGame_v) {}

GameNetMaster::~GameNetMaster() {
}

void GameNetMaster::Initialize() {
	CommonNetMaster::Initialize();

	const auto spGameNetGroup = MakeShared<GameNetGroup>();
	const auto spInterServerNetGroup = MakeShared<GameInterServerClientNetGroup>();

	AddNetGroup(NETGROUP_ID_MAIN,			spGameNetGroup);
	AddNetGroup(NETGROUP_ID_INTERSERVER,	spInterServerNetGroup);

	spGameNetGroup->Initialize();
	spInterServerNetGroup->Initialize();
}

void GameNetMaster::OnUpdate(const TimeSpan& elapsed) {

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

void GameNetMaster::OnCapturedInputEvent(int inputEvent) {
	CommonNetMaster::OnCapturedInputEvent(inputEvent);
}

void GameNetMaster::OnStopped() {
}

