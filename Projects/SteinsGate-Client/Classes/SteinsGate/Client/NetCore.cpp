/*
 * 작성자: 윤정도
 * 생성일: 5/6/2023 8:29:57 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetCore.h"

#include <SteinsGate/Client/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
NetCore::NetCore()
: netClientGroup_(nullptr)
, authTcp_(nullptr)
, lobbyTcp_(nullptr)
, logicTcp_(nullptr)
, logicUdp_(nullptr)
, chatTcp_(nullptr)
, areaTcp_(nullptr)
, commandSynchronizer_(CommandSynchronizer::Get())
, connectionSynchronizer_(ConnectionSynchronizer::Get())
, componentCollection_()
, authenticationComponent_(nullptr)
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
	netClientGroup_ = centerNetGroup.Get<NetClientGroup*>();
	netClientGroup_->Initialize();

	commandSynchronizer_->initialize(); // 이녀석은 무조건 IOCP 초기화이후 수행
	connectionSynchronizer_->initialize();

	authTcp_ = netClientGroup_->AuthTcp;
	lobbyTcp_ = netClientGroup_->LobbyTcp;
	logicTcp_ = netClientGroup_->LogicTcp;
	logicUdp_ = netClientGroup_->LogicUdp;
	chatTcp_ = netClientGroup_->ChatTcp;
	areaTcp_ = netClientGroup_->AreaTcp;

	authenticationComponent_ = dbg_new AuthenticationComponent;

	componentCollection_.add(authenticationComponent_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::Finalize()
{
	NetMaster::Finalize();

	JCORE_DELETE_SINGLETON_SAFE(commandSynchronizer_);
	JCORE_DELETE_SINGLETON_SAFE(connectionSynchronizer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::pollNetEvents()
{
	connectionSynchronizer_->processConnections();
	commandSynchronizer_->processCommands();
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::runCommand(SGSession* _pSession, ICommand* _pCmd)
{
	if (netClientGroup_->getParser()->RunCommand(_pSession, _pCmd) == false)
	{
		_NetLogWarn_("처리 불가능한 커맨드(%d) 수신", _pCmd->GetId());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::initializeComponents()
{
	componentCollection_.initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetCore::connectAuthTcp()
{
	if (authTcp_ == nullptr)
	{
		return false;
	}

	return authTcp_->ConnectAsync(Core::ServerProcessInfoPackage->auth_.remoteEp_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetCore::connectLobbyTcp()
{
	if (lobbyTcp_ == nullptr)
	{
		return false;
	}

	return lobbyTcp_->ConnectAsync(Core::ServerProcessInfoPackage->lobby_.remoteEp_);
}
