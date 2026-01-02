/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:52:07 PM
 * =====================
 *
 */

#include "Core.h"
#include "ServerCoreHeader.h"
#include "CommonNetGroup.h"

#include <sgs/CommonServer.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
CommonNetGroup::CommonNetGroup()
: pParser_(dbg_new CommandParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
CommonNetGroup::~CommonNetGroup()
{
	JC_DELETE_SAFE(pParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::Initialize()
{
	if (sg::ServerProcessInfo == nullptr)
	{
		_LogWarn_("서버 정보가 없어서 네트그룹 초기화 실패 [메인 네트그룹]");
		return;
	}

	InitializeBufferPool();
	InitializeIOCP();
	InitializeParser();
	InitializeServer();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::Finalize()
{
	NetGroup::Finalize();
	JC_DELETE_SAFE(pParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CommonNetGroup::AddUpdatable(int _id, IUpdatable* _pUpdatable)
{
	if (!updatableCollection_.Add(_id, _pUpdatable))
	{
		_LogWarn_("넷그룹 업데이터블 추가실패(%d)", _id);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::InitializeParser()
{
	// 서버 공통 커맨드 정의
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::ProcessUpdate(const TimeSpan& _elapsed)
{
	updatableCollection_.Update(_elapsed);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::ProcessOrder(CenterOrder_t _order)
{
	switch (_order)
	{
	case CenterOrder::LaunchServer:
		LaunchServer();
		break;
	case CenterOrder::StopServer:
		StopServer();
		break;
	default:
		jc_assert(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::LaunchServer()
{
	JC_LOCK_GUARD(serverBootLock_);
	const ServerBootState_t state = sg::CommonServer->GetBootState();

	if (state == ServerBootState::Launched || state == ServerBootState::Launching)
		return;

	sg::CommonServer->SetBootState(ServerBootState::Launching);
	sg::CommonServer->Start(sg::ServerProcessInfo->bindTcp_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroup::StopServer()
{
	JC_LOCK_GUARD(serverBootLock_);
	const ServerBootState_t state = sg::CommonServer->GetBootState();

	if (state == ServerBootState::Stopped || state == ServerBootState::Stopping)
		return;

	sg::CommonServer->SetBootState(ServerBootState::Stopping);
	sg::CommonServer->Stop();
}
