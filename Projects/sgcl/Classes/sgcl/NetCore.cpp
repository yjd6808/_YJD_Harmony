/*
 * 작성자: 윤정도
 * 생성일: 5/6/2023 8:29:57 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "NetCore.h"

#include <sg/Config.h>
#include <sgcl/AuthenticationComponent.h>

#include "sg/ClientInfo.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
NetCore::NetCore()
: pNetClientGroup_(nullptr)
, pAuthTcp_(nullptr)
, pLobbyTcp_(nullptr)
, pLogicTcp_(nullptr)
, pLogicUdp_(nullptr)
, pChatTcp_(nullptr)
, pAreaTcp_(nullptr)
, pCommandSynchronizer_(CommandSynchronizer::Get())
, pConnectionSynchronizer_(ConnectionSynchronizer::Get())
, componentCollection_()
, pAuthentication_(nullptr)
{
	SetName("클라");
}

//////////////////////////////////////////////////////////////////////////////////////////
NetCore::~NetCore()
{
	NetCore::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::Initialize()
{
	const auto centerNetGroup = MakeShared<NetClientGroup>();
	AddNetGroup(1, centerNetGroup);
	pNetClientGroup_ = centerNetGroup.Get<NetClientGroup*>();
	pNetClientGroup_->Initialize();

	pCommandSynchronizer_->Initialize(); // 이녀석은 무조건 IOCP 초기화이후 수행
	pConnectionSynchronizer_->Initialize();

	pAuthTcp_ = pNetClientGroup_->pAuthTcp_;
	pLobbyTcp_ = pNetClientGroup_->pLobbyTcp_;
	pLogicTcp_ = pNetClientGroup_->pLogicTcp_;
	pLogicUdp_ = pNetClientGroup_->pLogicUdp_;
	pChatTcp_ = pNetClientGroup_->pChatTcp_;
	pAreaTcp_ = pNetClientGroup_->pAreaTcp_;

	pAuthentication_ = dbg_new AuthenticationComponent;

	componentCollection_.Add(pAuthentication_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::Finalize()
{
	NetGroupMgr::Finalize();

	JCORE_DELETE_SINGLETON_SAFE(pCommandSynchronizer_);
	JCORE_DELETE_SINGLETON_SAFE(pConnectionSynchronizer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::PollNetEvents()
{
	pConnectionSynchronizer_->ProcessConnections();
	pCommandSynchronizer_->ProcessCommands();
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::RunCommand(jnet::Session* _pSession, ICommand* _pCmd)
{
	if (pNetClientGroup_->GetParser()->RunCommand(_pSession, _pCmd) == false)
	{
		_NetLogWarn_("처리 불가능한 커맨드(%d) 수신", _pCmd->GetId());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::InitializeComponents()
{
	componentCollection_.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetCore::ConnectAuthTcp()
{
	if (pAuthTcp_ == nullptr)
	{
		return false;
	}

	return pAuthTcp_->ConnectAsync(g_cConfigRuntime.GetClientInfo()->remoteAuth_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetCore::ConnectLobbyTcp()
{
	if (pLobbyTcp_ == nullptr)
	{
		return false;
	}

	return pLobbyTcp_->ConnectAsync(g_cConfigRuntime.GetClientInfo()->remoteLobby_);
}
