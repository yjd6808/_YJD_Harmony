/*
 * 작성자: 윤정도
 * 생성일: 5/6/2023 8:29:57 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetCore.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

NetCore::NetCore()
	: Group(nullptr)
	, AuthTcp(nullptr)
	, LobbyTcp(nullptr)
	, GameTcp(nullptr)
	, GameUdp(nullptr)
	, AreaTcp(nullptr)
	, ChatTcp(nullptr)
	, m_pCommandSynchronizer(CommandSynchronizer::Get())
{}

NetCore::~NetCore() {
	DeleteSingletonSafe(m_pCommandSynchronizer);
}

void NetCore::Initialize() {
	const auto spCenterNetGroup = MakeShared<NetClientGroup>();
	AddNetGroup(1, spCenterNetGroup);
	Group = spCenterNetGroup.Get<NetClientGroup*>();
	Group->Initialize();
	m_pCommandSynchronizer->initialize();	// 무조건 IOCP 초기화이후 수행

	AuthTcp = Group->AuthTcp;
	LobbyTcp = Group->LobbyTcp;
	GameTcp = Group->GameTcp;
	GameUdp = Group->GameUdp;
	ChatTcp = Group->ChatTcp;
	AreaTcp = Group->AreaTcp;

	
}


void NetCore::pollNetEvents() {
	m_pCommandSynchronizer->processCommands();
}

void NetCore::runCommand(Session* session, ICommand* cmd) {
	if (Group->m_Parser.RunCommand(session, cmd) == false) {
		_NetLogWarn_("처리 불가능한 커맨드(%d) 수신", cmd->GetCommand());
	}
}

