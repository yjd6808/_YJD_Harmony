#pragma once

#include <JNetwork/Packet.h>
#include <Common/Structure.h>

// TCP Commands
#define LOGIN_SYN						100		// Ŭ�� -> ����
#define LOGIN_ACK						101		// ���� -> Ŭ��

#define REGISTER_SYN					102		// Ŭ�� -> ����
#define REGISTER_ACK					103		// ���� -> Ŭ��

#define LOAD_CHANNEL_INFO_SYN			104		// Ŭ�� -> ���� / (ä�� ���Խ�)
#define LOAD_CHANNEL_INFO_ACK			105		// ���� -> Ŭ��

#define SELECT_CHANNEL_SYN				106		// Ŭ�� -> ���� / (ChannelLayer���� ä�� ��ư Ŭ����)
#define SELECT_CHANNEL_ACK				107		// ���� -> Ŭ��

#define LOAD_CHARACTER_INFO_SYN			108		// Ŭ�� -> ���� / (ĳ���� ���� ���Խ�)
#define LOAD_CHARACTER_INFO_ACK			109		// ���� -> Ŭ��

#define CREATE_CHARACTER_SYN			110		// Ŭ�� -> ���� / (CharacterSelectLayer���� ĳ���� ���� ��ư Ŭ���� ����)
#define CREATE_CHARACTER_ACK			111		// ���� -> Ŭ��

#define DELETE_CHARACTER_SYN			112		// Ŭ�� -> ���� / (CharacterSelectLayer���� ĳ���� ���� ��ư Ŭ���� ����)
#define DELETE_CHARACTER_ACK			113		// ���� -> Ŭ��

#define SELECT_CHARACTER_SYN			114		// Ŭ�� -> ���� / (CharacterSelectLayer���� �ε��� ĳ���� ��ư Ŭ���� ����)
#define SELECT_CHARACTER_ACK			115		// ���� -> Ŭ��

#define JOIN_LOBBY_SYN					116		// Ŭ�� -> ���� / (�κ� ���Խ�) ���� �κ� ������ ��û�Ѵ�.
#define UPDATE_CHARACTER_INFO_ACK		117		// ���� -> Ŭ�� / ������Ʈ�� ĳ���� ������ �����Ѵ�.
#define UPDATE_ROOMLIST_ACK				118		// ���� -> Ŭ�� / ����Ʈ�� �� �������� �����Ѵ�.
#define UPDATE_FRIENDSHOP_INFO_ACK		119		// ���� -> Ŭ�� / ģ�� ���� ������ �����Ѵ�.

#define HARD_DISCONNECT_SYN				250		// ���� -> Ŭ�� / ������ �ش� Ŭ���� ������ ������ ���� ���

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

// �ִ� 10���� ä�� �������� ���� �� �ֵ���.
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



// ���� �ֱ⸶�� Ŭ�� TCP ������ ���� �����Ѵ�.
// ������ �̸� �����ϰ� ������ �ð��� Ŭ���̾�Ʈ ������ ����Ѵ�.
// ������ ���������� Ŭ���̾�Ʈ�� �ð��� üũ�ϰ� �������� �ð����� �ʴ� ��� ������ ������ ����������� �Ѵ�.
// �̷� ������ ȥ�������� ���� ���� �� ���� ������?
struct TcpPingSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpPingSyn, TCP_PING_SYN)

	Int32 UID;
	Int64U Tick;
};

// ���� �����ϸ� ��ٷ� ��󿡰� Ack�� �������ش�.
// Ŭ��� �̸� �����ϰ� Ŭ���̾�Ʈ�� ����Ѵ�. Ŭ�� ������ �ð����� ACK�� �������� ������ ������ ������ �Ѵ�.
struct TcpPingAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpPingAck, TCP_PING_ACK)
};

// Ŭ��� �����ֱ⸶�� ������ �ڽ��� ��ġ/�ӵ����� ������ ������ �����Ѵ�.
// ������ �ش� UUID�� Ŭ���̾�Ʈ�� ���������� �����Ѵ�.
struct TcpTankMoveSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpTankMoveSyn, TANK_MOVE_SYN)

	Int32 UUID;
	TankMove Move;
};

// ������ �����ֱ⸶�� Ŭ���̾�Ʈ���� ��ġ������ Ŭ���̾�Ʈ��� �������ֵ����Ѵ�.
struct TcpTankMoveAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpTankMoveAck, TANK_MOVE_ACK)

	Int32 UUID;
	TankMove Move;
};