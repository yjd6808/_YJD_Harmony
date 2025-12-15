/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterNetMaster.h"

#include <SteinsGate/Common/ServerConst.h>

#include <SteinsGate/Server/CenterNetGroup.h>
#include <SteinsGate/Server/CenterInterServerClientNetGroup.h>

USING_NS_JC;
USING_NS_JNET;

CenterNetMaster::CenterNetMaster()
	: CommonNetMaster(Const::Process::LoopPerSecondCenter)
{
	SetName("센터");
}

CenterNetMaster::~CenterNetMaster()
{
}

void CenterNetMaster::Initialize()
{
	CommonNetMaster::Initialize();

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

