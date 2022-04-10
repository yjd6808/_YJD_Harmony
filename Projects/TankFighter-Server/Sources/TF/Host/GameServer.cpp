/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
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
	m_Parser.AddCommand(SELECT_CHANNEL_SYN, CommandFn::CmdSelectChannelSyn);

	m_Parser.AddCommand(LOAD_CHARACTER_INFO_SYN, CommandFn::CmdLoadCharacterInfoSyn);
	m_Parser.AddCommand(SELECT_CHARACTER_SYN, CommandFn::CmdSelectCharacterSyn);
	m_Parser.AddCommand(DELETE_CHARACTER_SYN, CommandFn::CmdDeleteCharacterSyn);
	m_Parser.AddCommand(CREATE_CHARACTER_SYN, CommandFn::CmdCreateCharacterSyn);

	m_Parser.AddCommand(JOIN_LOBBY_SYN, CommandFn::CmdJoinLobbySyn);
	m_Parser.AddCommand(CREATE_ROOM_SYN, CommandFn::CmdCreateRoomSyn);
	m_Parser.AddCommand(JOIN_ROOM_SYN, CommandFn::CmdJoinRoomSyn);
	m_Parser.AddCommand(ADD_FRIEND_SYN, CommandFn::CmdAddFriendSyn);
	m_Parser.AddCommand(ADD_FRIEND_REQUEST_ACK, CommandFn::CmdAddFriendRequestAck);
	m_Parser.AddCommand(DELETE_FRIEND_SYN, CommandFn::CmdDeleteFriendSyn);
	m_Parser.AddCommand(LOAD_ROOM_INFO_SYN, CommandFn::CmdLoadRoomInfoSyn);
	m_Parser.AddCommand(ROOM_GAME_START_SYN, CommandFn::CmdRoomGameStartSyn);
	m_Parser.AddCommand(ROOM_GAME_READY_SYN, CommandFn::CmdRoomGameReadySyn);
	m_Parser.AddCommand(ROOM_GAME_READY_CANCEL_SYN, CommandFn::CmdRoomGameReadyCancelSyn);
	m_Parser.AddCommand(ROOM_LEAVE_SYN, CommandFn::CmdRoomLeaveSyn);

	m_Parser.AddCommand(BATTLE_FIELD_LOAD_SYN, CommandFn::CmdBattleFieldLoadSyn);
	m_Parser.AddCommand(BATTLE_FIELD_TANK_MOVE_SYN, CommandFn::CmdBattileFieldTankMoveSyn);
	m_Parser.AddCommand(BATTLE_FIELD_LEAVE_SYN, CommandFn::CmdBattleFieldLeaveSyn);

	m_Parser.AddCommand(CHAT_MESSAGE_SYN, CommandFn::CmdChatMessageSyn);
	m_Parser.AddCommand(BATTLE_FIELD_FIRE_SYN, CommandFn::CmdBattleFieldFireSyn);
	m_Parser.AddCommand(BATTLE_FIELD_DEATH_SYN, CommandFn::CmdBattleFieldDeathSyn);

	m_Parser.AddCommand(TCP_RTT_SYN, CommandFn::CmdTcpRTTSyn);

	this->SetEventListener(m_pListener);

	return GameServer::Start(StringUtil::Format("%s:%d", TCP_BIND_ADDR, TCP_BIND_PORT).Source());
}


