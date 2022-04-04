#define _WINSOCKAPI_		// winsock.h 연속 include 방지
							// 아니 근데, 에바아닙니까 ㄹㅇ; 이거 땜에 1시간 동안 손발 덜덜떨렸네

#include <TF/Host/GameServer.h>
#include <TF/Host/GameServerEventListener.h>
#include <TF/ServerConfiguration.h>

#include <JCore/AutoObject.h>
#include <TF/Parser/CommandFn.h>
#include <Common/Command.h>

using namespace JCore;

GameServer* GameServer::ms_pInstance = nullptr;

GameServer* GameServer::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new GameServer();
		ms_pInstance->m_pListener = new GameServerEventListener(&ms_pInstance->m_Parser);
	}

	return ms_pInstance;
}

GameServer::~GameServer() {
	DeleteSafe(m_pListener);
}


bool GameServer::StartServer() {

	m_Parser.AddCommand(LOGIN_SYN, CommandFn::CmdLoginSyn);
	m_Parser.AddCommand(REGISTER_SYN, CommandFn::CmdRegisterSyn);
	m_Parser.AddCommand(LOAD_CHANNEL_INFO_SYN, CommandFn::CmdLoadChannelInfoSyn);
	m_Parser.AddCommand(LOAD_CHARACTER_INFO_SYN, CommandFn::CmdLoadCharacterInfoSyn);
	m_Parser.AddCommand(SELECT_CHARACTER_SYN, CommandFn::CmdSelectCharacterSyn);
	m_Parser.AddCommand(DELETE_CHARACTER_SYN, CommandFn::CmdDeleteCharacterSyn);
	m_Parser.AddCommand(CREATE_CHARACTER_SYN, CommandFn::CmdCreateCharacterSyn);

	m_Parser.AddCommand(TANK_MOVE_SYN, CommandFn::CmdTankMoveSyn);
	m_Parser.AddCommand(TANK_MOVE_ACK, CommandFn::CmdTankMoveAck);

	this->SetEventListener(m_pListener);

	return GameServer::Start(StringUtil::Format("%s:%d", TCP_BIND_ADDR, TCP_BIND_PORT).Source());
}


