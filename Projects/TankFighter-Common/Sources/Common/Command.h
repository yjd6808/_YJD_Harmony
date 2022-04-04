#pragma once

#include <JNetwork/Packet.h>
#include <Common/Structure.h>

// TCP Commands
#define LOGIN_SYN						100		// 클라 -> 서버
#define LOGIN_ACK						101		// 서버 -> 클라

#define REGISTER_SYN					102		// 클라 -> 서버
#define REGISTER_ACK					103		// 서버 -> 클라

#define LOAD_CHANNEL_INFO_SYN			104		// 클라 -> 서버 / (채널 진입시)
#define LOAD_CHANNEL_INFO_ACK			105		// 서버 -> 클라

#define SELECT_CHANNEL_SYN				106		// 클라 -> 서버 / (ChannelLayer에서 채널 버튼 클릭시)
#define SELECT_CHANNEL_ACK				107		// 서버 -> 클라

#define LOAD_CHARACTER_INFO_SYN			108		// 클라 -> 서버 / (캐릭터 선택 진입시)
#define LOAD_CHARACTER_INFO_ACK			109		// 서버 -> 클라

#define CREATE_CHARACTER_SYN			110		// 클라 -> 서버 / (CharacterSelectLayer에서 캐릭터 생성 버튼 클릭시 전송)
#define CREATE_CHARACTER_ACK			111		// 서버 -> 클라

#define DELETE_CHARACTER_SYN			112		// 클라 -> 서버 / (CharacterSelectLayer에서 캐릭터 제거 버튼 클릭시 전송)
#define DELETE_CHARACTER_ACK			113		// 서버 -> 클라

#define SELECT_CHARACTER_SYN			114		// 클라 -> 서버 / (CharacterSelectLayer에서 로딩된 캐릭터 버튼 클릭시 전송)
#define SELECT_CHARACTER_ACK			115		// 서버 -> 클라

#define JOIN_LOBBY_SYN					116		// 클라 -> 서버 / (로비 진입시) 각종 로비 정보를 요청한다.
#define UPDATE_CHARACTER_INFO_ACK		117		// 서버 -> 클라 / 업데이트된 캐릭터 정보를 전송한다.
#define UPDATE_ROOMLIST_ACK				118		// 서버 -> 클라 / 업데트된 방 정보들을 전송한다.
#define UPDATE_FRIENDSHOP_INFO_ACK		119		// 서버 -> 클라 / 친구 관계 정보를 전송한다.

#define HARD_DISCONNECT_SYN				250		// 서버 -> 클라 / 서버가 해당 클라의 연결을 강제로 끊는 경우

#define TANK_MOVE_SYN					150
#define TANK_MOVE_ACK					151

#define TCP_PING_SYN					200
#define TCP_PING_ACK					201

// UDP Commands
#define UDP_PING_SYN					2000
#define UDP_PING_ACK					2001


#define CMD_DEFAULT_CONSTRUCTOR(__struct__, __cmd__) \
	__struct__() {									 \
		Cmd = __cmd__;								 \
		CmdLen = sizeof(__struct__);				 \
	}




struct LoginSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoginSyn, LOGIN_SYN)

	char Id[ID_LEN]{};
	char Password[PASS_LEN]{};
};

struct LoginAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoginAck, LOGIN_ACK)

	int UID = INVALID_UID;
	bool Result;
	char Reason[REASON_LEN]{};
};

struct RegisterSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RegisterSyn, REGISTER_SYN)

	char Id[ID_LEN]{};
	char Password[PASS_LEN]{};
};

struct RegisterAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RegisterAck, REGISTER_ACK)

	bool Result;
	char Reason[REASON_LEN]{};
};

struct LoadChannelInfoSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoadChannelInfoSyn, LOAD_CHANNEL_INFO_SYN)

	// Empty
};

// 최대 10개의 채널 정보까지 받을 수 있도록.
struct LoadChannelInfoAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoadChannelInfoAck, LOAD_CHANNEL_INFO_ACK)

	int Count = 0;
	ChannelInfo Info[10];
};


struct SelectChannelSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(SelectChannelSyn, SELECT_CHANNEL_SYN)

	int ChanneldUID = INVALID_UID;
};

struct SelectChannelAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(SelectChannelAck, SELECT_CHANNEL_ACK)

	bool Result{};
	char Reason[REASON_LEN]{};
	int ChanneldUID{};
};

struct LoadCharacterInfoSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoadCharacterInfoSyn, LOAD_CHARACTER_INFO_SYN)

	int AccountUID = INVALID_UID;
	int ChannelUID = INVALID_UID;
};

struct LoadCharacterInfoAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoadCharacterInfoAck, LOAD_CHARACTER_INFO_ACK)

	int Count;
	CharacterInfo Info[10];
};

struct CreateCharacterSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(CreateCharacterSyn, CREATE_CHARACTER_SYN)

	int AccountUID = INVALID_UID;
	int ChannelUID = INVALID_UID;
	char CharacterName[NAME_LEN];
};

struct CreateCharacterAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(CreateCharacterAck, CREATE_CHARACTER_ACK)

	bool Result{};
	char Reason[REASON_LEN]{};
};

struct DeleteCharacterSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(DeleteCharacterSyn, DELETE_CHARACTER_SYN)

	int AccountUID = INVALID_UID;
	int ChannelUID = INVALID_UID;
	char CharacterName[NAME_LEN];
};

struct DeleteCharacterAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(DeleteCharacterAck, DELETE_CHARACTER_ACK)

	bool Result{};
	char Reason[REASON_LEN]{};
};

struct SelectCharacterSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(SelectCharacterSyn, SELECT_CHARACTER_SYN)

	int AccountUID = INVALID_UID;
	int ChannelUID = INVALID_UID;
	int CharacterUID = INVALID_UID;
};

struct SelectCharacterAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(SelectCharacterAck, SELECT_CHARACTER_ACK)

	int CharacterUID = INVALID_UID;
	bool Result{};
	char Reason[REASON_LEN]{};
};

struct HardDisconnectSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(HardDisconnectSyn, HARD_DISCONNECT_SYN)
	char Reason[REASON_LEN]{};
};



// 일정 주기마다 클라가 TCP 서버로 핑을 전송한다.
// 서버는 이를 수신하고 수신한 시각을 클라이언트 정보에 기록한다.
// 서버는 지속적으로 클라이언트의 시간을 체크하고 핑전송이 시간내로 않는 경우 강제로 연결을 끊어버리도록 한다.
// 이러 서버가 혼잡해지는 것을 막을 수 있지 않을까?
struct TcpPingSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpPingSyn, TCP_PING_SYN)

	Int32 UID;
	Int64U Tick;
};

// 핑을 수신하면 곧바로 대상에게 Ack를 전송해준다.
// 클라는 이를 수신하고 클라이언트에 기록한다. 클라도 정해진 시간내로 ACK를 수신하지 않으면 연결을 끊도록 한다.
struct TcpPingAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpPingAck, TCP_PING_ACK)
};

// 클라는 일정주기마다 서버로 자신의 위치/속도등의 정보를 서버로 전송한다.
// 서버는 해당 UUID의 클라이언트의 상태정보를 저장한다.
struct TcpTankMoveSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpTankMoveSyn, TANK_MOVE_SYN)

	Int32 UUID;
	TankMove Move;
};

// 서버는 일정주기마다 클라이언트들의 위치정보를 클라이언트들로 전송해주도록한다.
struct TcpTankMoveAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpTankMoveAck, TANK_MOVE_ACK)

	Int32 UUID;
	TankMove Move;
};