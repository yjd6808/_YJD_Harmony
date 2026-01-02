/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthNetMaster.h"

#include <sgs/ServerConst.h>

#include <sgs_auth/AuthNetGroup.h>
#include <sgs_auth/AuthInterServerClientNetGroup.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
AuthNetMaster::AuthNetMaster() : CommonNetGroupMgr(Const::Process::LoopPerSecondAuth)
{
	SetName("인증");
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthNetMaster::~AuthNetMaster()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthNetMaster::Initialize()
{
	CommonNetGroupMgr::Initialize();

	const auto pAuthNetGroup = MakeShared<AuthNetGroup>();
	const auto pInterServerNetGroup = MakeShared<AuthInterServerClientNetGroup>();

	AddNetGroup(Const::NetGroup::MainId, pAuthNetGroup);
	AddNetGroup(Const::NetGroup::InterServerId, pInterServerNetGroup);

	pAuthNetGroup->Initialize();
	pInterServerNetGroup->Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthNetMaster::OnUpdate(const TimeSpan& _elapsed)
{
#if _DEBUG
	static TimeCounter s;
	s.Elapsed += _elapsed;

	if (s.ElapsedSeconds(5))
	{
		//_LogDebug_("중앙 시각 %s", sg::Contents.TimeManager->Now().FormatMysqlTime().Source());
		//_LogDebug_("현재 시각 %s", DateTime::Now().FormatMysqlTime().Source());
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthNetMaster::OnStopped()
{
}
