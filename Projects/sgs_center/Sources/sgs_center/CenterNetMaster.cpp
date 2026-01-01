/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Core.h"
#include "CenterCoreHeader.h"
#include "CenterNetMaster.h"

#include <sgs/ServerConst.h>

#include <sgs_center/CenterNetGroup.h>
#include <sgs_center/CenterInterServerClientNetGroup.h>

USING_NS_JC;
USING_NS_JNET;

CenterNetMaster::CenterNetMaster()
: CommonNetGroupMgr(Const::Process::LoopPerSecondCenter)
{
	SetName("센터");
}

CenterNetMaster::~CenterNetMaster()
{
}

void CenterNetMaster::Initialize()
{
	CommonNetGroupMgr::Initialize();

	const auto pCenterNetGroup = MakeShared<CenterNetGroup>();
	const auto pInterServerNetGroup = MakeShared<CenterInterServerClientNetGroup>();

	AddNetGroup(Const::NetGroup::MainId, pCenterNetGroup);
	AddNetGroup(Const::NetGroup::InterServerId, pInterServerNetGroup);

	pCenterNetGroup->Initialize();
	pInterServerNetGroup->Initialize();
}

void CenterNetMaster::OnUpdate(const TimeSpan& _elapsed)
{
}

void CenterNetMaster::OnStopped()
{
}

