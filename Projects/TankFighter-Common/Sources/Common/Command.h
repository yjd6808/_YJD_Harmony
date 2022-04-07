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
#define UPDATE_FRIENDLIST_ACK			119		// ���� -> Ŭ�� / ģ�� ���� ������ �����Ѵ�.

#define CREATE_ROOM_SYN					120		// Ŭ�� -> ���� / �� ���� ��ư Ŭ����
#define CREATE_ROOM_ACK					124		// ���� -> Ŭ��

#define JOIN_ROOM_SYN					121		// Ŭ�� -> ���� / �� ���� / �� ���(����Ʈ��)�� �� ��ư Ŭ����
#define JOIN_ROOM_ACK					125		// ���� -> Ŭ��

#define UPDATE_ROOMINFO_ACK				126		// ���� -> Ŭ�� / �ٸ� ������ �濡 �����ϰų�, �濡 �ִ� ������ ������ ���

#define ADD_FRIEND_SYN					122		// Ŭ�� -> ���� / ģ�� �߰� ��ư Ŭ����
#define ADD_FRIEND_ACK					127		// ���� -> Ŭ�� / ģ�� �߰� ��û�� ���� ����� ������.
#define ADD_FRIEND_REQUEST_SYN			128		// ���� -> Ŭ�� / ģ�� �߰� ��û ��󿡰� ��û������ �����Ѵ�.
#define ADD_FRIEND_REQUEST_ACK			128		// Ŭ�� -> ���� / ģ�� ��û�� ���� Ŭ���̾�Ʈ�� ����/�ź��� ����� ������ �����Ѵ�.
// #define ADD_FRIEND_REQUESTR_RESULT_SYN  129	// ���� -> Ŭ�� / ģ�� ��û�ڿ��� ��û ����� �����Ѵ�. - �ʿ� ����


#define DELETE_FRIEND_SYN				123		// Ŭ�� -> ���� / ģ�� ���� / ģ�� ���(����Ʈ��)�� ģ�� ��ư Ŭ����
#define DELETE_FRIEND_ACK				130		// ���� -> Ŭ�� / ������ ����� �������� ��� ��󿡰Ե� �����Ǿ��ٰ� �۽��ϰ� ������ ������Ե� �۽��Ѵ�. �׸��� ģ�� ��ϵ� ��������

#define LOAD_ROOM_INFO_SYN				131		// Ŭ�� -> ���� / (�� ���� ��) ���� �� ������ ��û�Ѵ�.
#define LOAD_ROOM_INFO_ACK				132		// ���� -> Ŭ��



#define ROOM_GAME_START_SYN				133		// Ŭ�� -> ���� / ���� ���� ��ư Ŭ��
#define ROOM_GAME_START_ACK				137		// ���� -> Ŭ�� / �� ���� ����鿡�� ���� ���� ��Ŷ ����

#define ROOM_GAME_READY_SYN				134		// Ŭ�� -> ���� / ���� �غ� ��ư Ŭ��
#define ROOM_GAME_READY_ACK				137		// ���� -> Ŭ�� / �غ� ��û�� ���� ����

#define ROOM_GAME_READY_CANCEL_SYN		135		// Ŭ�� -> ���� / �غ� ���� ��ư Ŭ��
#define ROOM_GAME_READY_CANCEL_ACK		138		// ���� -> Ŭ�� / �غ� ������ ���� ����

#define ROOM_LEAVE_SYN					136		// Ŭ�� -> ���� / �� ������ ��ư Ŭ��
#define ROOM_LEAVE_ACK					139		// ���� -> Ŭ�� / �� �����⿡ ���� ����

#define SERVER_MESSAGE_SYN				400		// ���� -> Ŭ��� Ư�� �޽��� ����

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
	ChannelInfo Info[COMMAND_ARRAY_LEN];
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
	CharacterInfo Info[COMMAND_ARRAY_LEN];
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

	int AccountUID		= INVALID_UID;
	int ChannelUID		= INVALID_UID;
	char CharacterName[NAME_LEN];
};

struct DeleteCharacterAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(DeleteCharacterAck, DELETE_CHARACTER_ACK)

	char Reason[REASON_LEN]{};
};

struct SelectCharacterSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(SelectCharacterSyn, SELECT_CHARACTER_SYN)

	int AccountUID		= INVALID_UID;
	int ChannelUID		= INVALID_UID;
	int CharacterUID	= INVALID_UID;
};

struct SelectCharacterAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(SelectCharacterAck, SELECT_CHARACTER_ACK)

	int CharacterUID = INVALID_UID;
	bool Result{};
	char Reason[REASON_LEN]{};
};



struct JoinLobbySyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(JoinLobbySyn, JOIN_LOBBY_SYN)

	int AccountUID = INVALID_UID;
	int ChannelUID = INVALID_UID;
	int CharacterUID = INVALID_UID;
};

struct UpdateCharacterInfoAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(UpdateCharacterInfoAck, UPDATE_CHARACTER_INFO_ACK)

	bool Result{};
	char Reason[REASON_LEN];
	CharacterInfo Info{};
};

struct UpdateRoomListAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(UpdateRoomListAck, UPDATE_ROOMLIST_ACK)

	int Count = 0;
	bool Result{};
	char Reason[REASON_LEN];
	RoomInfo Info[COMMAND_ARRAY_LEN];
};

struct UpdateFriendListAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(UpdateFriendListAck, UPDATE_FRIENDLIST_ACK)

	int Count = 0;
	CharacterInfo Info[COMMAND_ARRAY_LEN];
};

struct CreateRoomSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(CreateRoomSyn, CREATE_ROOM_SYN)

	char RoomName[NAME_LEN];
};

struct CreateRoomAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(CreateRoomAck, CREATE_ROOM_ACK)

	bool Result{};
	int RoomUID = INVALID_UID;
	char Reason[REASON_LEN];
};



struct JoinRoomSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(JoinRoomSyn, JOIN_ROOM_SYN)

	int RoomUID = INVALID_UID;
};


struct JoinRoomAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(JoinRoomAck, JOIN_ROOM_ACK)

	int RoomUID = INVALID_UID;
	bool Result{};
	char Reason[REASON_LEN];
};


struct UpdateRoomInfoAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(UpdateRoomInfoAck, UPDATE_ROOMINFO_ACK)

	int Count = 0;
	int HostCharacterUID = INVALID_UID;		// ����
	RoomCharacterInfo Info[ROOM_MAX_PLAYER_COUNT];
};


struct AddFriendSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(AddFriendSyn, ADD_FRIEND_SYN)

	char FriendName[NAME_LEN];
};

struct AddFriendAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(AddFriendAck, ADD_FRIEND_ACK)

	bool Result;
	char Reason[REASON_LEN];
};

struct AddFriendRequestSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(AddFriendRequestSyn, ADD_FRIEND_REQUEST_SYN)

	int RequestCharacterUID;	// ģ�� ��û�� ���� ����� UID
	CharacterInfo Info;
};

struct AddFriendRequestAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(AddFriendRequestAck, ADD_FRIEND_REQUEST_ACK)

	bool Accept = false;				// ģ�� ��û ����/�ź� ����
	int RequestCharacterUID;
	int AcceptedCharacterUID;
};

struct DeleteFriendSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(DeleteFriendSyn, DELETE_FRIEND_SYN)

	int DeleteCharacterUID = INVALID_UID;
};


struct DeleteFriendAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(DeleteFriendAck, DELETE_FRIEND_ACK)

	
	bool Result = false;				// ģ�� ���� ����/���� ����
	char Reason[REASON_LEN];
};

struct ServerMessageSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(ServerMessageSyn, SERVER_MESSAGE_SYN)

	char Message[MESSAGE_LEN];
};


struct LoadRoomInfoSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoadRoomInfoSyn, LOAD_ROOM_INFO_SYN)

	int AccountUID		= INVALID_UID;
	int ChannelUID		= INVALID_UID;
	int CharacterUID	= INVALID_UID;
	int RoomUID			= INVALID_UID;
};

struct LoadRoomInfoAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(LoadRoomInfoAck, LOAD_ROOM_INFO_ACK)

	bool Result;
	char Reason[REASON_LEN];
	RoomInfo Info;
};


struct RoomGameStartSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomGameStartSyn, ROOM_GAME_START_SYN)

	int AccountUID		= INVALID_UID;
	int ChannelUID		= INVALID_UID;
	int CharacterUID	= INVALID_UID;
	int RoomUID			= INVALID_UID;
};

struct RoomGameStartAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomGameStartAck, ROOM_GAME_START_ACK)

	bool Result;
	char Reason[REASON_LEN];
};


struct RoomGameReadySyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomGameReadySyn, ROOM_GAME_READY_SYN)
};

struct RoomGameReadyCancelSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomGameReadyCancelSyn, ROOM_GAME_READY_CANCEL_SYN)
};


struct RoomLeaveSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomLeaveSyn, ROOM_LEAVE_SYN)
};

struct RoomLeaveAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomLeaveAck, ROOM_LEAVE_ACK)

	bool Result;
	char Reason[REASON_LEN];
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