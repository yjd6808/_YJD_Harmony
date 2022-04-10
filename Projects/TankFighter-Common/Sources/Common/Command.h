/*
 * �ۼ��� : ������
 *
 * Syn : ����
 * Ack : �����ſ� ���� ����
 *
 * �̷��� �����غôµ� ���ΰ���..
 *
 * ����
 * 1. �̸��� �� �������. �Ѵ��� �� �ȵ���
 * 2. �ö� ���� �Ȱ��� ������ ���� �� �ִ�
 */

#pragma once

#include <JNetwork/Packet.h>
#include <Common/Structure.h>
#include <Common/GameConfiguration.h>

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

#define BATTLE_FIELD_LOAD_SYN			150		// Ŭ�� -> ���� / �濡 �����ϸ� ������ �� �غ�Ƽ�! ��� �˷��ش�.
#define BATTLE_FIELD_LOAD_ACK			151		// ���� ���۱��� ���� �ð��� �� �����׸��� �� �÷��̾ ������ ��ġ ������ �����ش�.

#define BATTLE_FIELD_TANK_MOVE_SYN		161		// Ŭ�� -> ���� / Ŭ�󿡼� �����϶����� ���� �ֱ⸶�� ������ �ڽ��� ��ġ�� �����Ѵ�. - ������ �̸� ������Ʈ�Ѵ�.
#define BATTLE_FIELD_TANK_UPDATE_SYN	162		// ���� -> Ŭ�� / ������ �����ֱ⸶�� �÷��̾� ��ġ�� �÷������� ���� �����鿡�� ��ε�ĳ���� ���ش�.
#define BATTLE_FIELD_PLAYWAIT_END_SYN	163		// ���� -> Ŭ�� / ���� ���� ��� ���°� ������� ���� �����ص� �ȴٰ� �˷��ش�. (���� ���� �� �� ����)
#define BATTLE_FIELD_PLAYING_END_SYN	164		// ���� -> Ŭ�� / ���� ���� �ð��� ���� ��� �˷��ش�. (���� ���� ����)
#define BATTLE_FIELD_ENDWAIT_END_SYN	165		// ���� -> Ŭ�� / ���� ������ �ð��� ���� ��� �˷��ش�. ��� �÷��̾ �κ� ���·� �������ش�.

#define BATTLE_FIELD_FIRE_SYN			166		// Ŭ�� -> ���� / �÷��̾� �Ѿ� �߻� �� ��ġ ���� ����
#define BATTLE_FIELD_FIRE_ACK			167		// ���� -> Ŭ�� / �Ѿ� �߻��ߴٰ� ��ε�ĳ����

#define BATTLE_FIELD_DEATH_SYN			168		// Ŭ�� -> ���� / 
#define BATTLE_FIELD_DEATH_ACK			169		// ���� -> Ŭ�� / �ٸ� �÷��̾�� ��ε�ĳ���� �� ���ο� �ش��ϴ� ��� �����̹� Ÿ�̸� ������ �����Ѵ�.
#define BATTLE_FIELD_REVIVAL_SYN		170		// ���� -> Ŭ�� / ��Ȱ�ð��� �ٵǼ� ĳ���͸� ��������Ų��.
#define BATTLE_FILED_STATISTICS_UPDATE_SYN 171	// ���� -> Ŭ�� / �ֱ������� 

#define BATTLE_FIELD_LEAVE_SYN			180		// Ŭ�� -> ���� / �泪���� Ŭ��
#define BATTLE_FIELD_LEAVE_ACK			181		// Ŭ�� -> ���� / ������ �����ϰ� �����ִ� ��鿡�� �����ٰ� �˷���

#define SERVER_MESSAGE_SYN				400		// ���� -> Ŭ��� Ư�� �޽��� ����
#define CHAT_MESSAGE_SYN				401		// Ŭ�� -> ä�� �޽��� ����
#define CHAT_MESSAGE_ACK				402		// ���� -> Ŭ�� ��ε�ĳ��Ʈ ����

#define TCP_RTT_SYN						200		// Ŭ�� -> ���� / RTT/�����Ͻ� ����� ���� ���� �ð��� ��Ƽ� ����
#define TCP_RTT_ACK						201		// ���� -> Ŭ�� / 

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


// ���� ����/���� ���� ��� ���ÿ� ó���ϵ��� ��
// ��Ŷ �ϳ��� �߰��ϸ� �Ǵµ� �����⵵ �ϰ� �� �̷�ó����
struct RoomGameStartSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomGameStartSyn, ROOM_GAME_START_SYN)

	int AccountUID		= INVALID_UID;
	int ChannelUID		= INVALID_UID;
	int CharacterUID	= INVALID_UID;
	int RoomUID			= INVALID_UID;
	bool Intrude		= false;
};

// ���� �������� ������ ��� Result�� Reason�� ����
// ���� �������� ������ ��� BattleFieldScene���� �ش� ��Ŷ�� ��� �÷��̾ �����Ͽ� �����ڸ� Ȯ���� �� �ֵ��� �Ѵ�.
struct RoomGameStartAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(RoomGameStartAck, ROOM_GAME_START_ACK)

	RoomCharacterInfo IntruderInfo{};
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


struct BattleFieldLoadSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldLoadSyn, BATTLE_FIELD_LOAD_SYN);
};

struct BattleFieldLoadAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldLoadAck, BATTLE_FIELD_LOAD_ACK);

	RoomState	RoomState;		// �����
	float		LeftTime;		// (�ʴ���) ���� ������� �����ð� - �̽ð����� �ٸ� �÷��̾ �����Ҷ����� ��ٸ�
	TankMove	InitialMove;	// �÷��̾� �ʱ� ��ġ ����
};


//Ŭ��� �����ֱ⸶�� ������ �ڽ��� ��ũ ������ ������ �����Ѵ�.
//������ �� ������ �޾Ƽ� ������Ʈ �����ش�.
struct BattileFieldTankMoveSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattileFieldTankMoveSyn, BATTLE_FIELD_TANK_MOVE_SYN)

	TankMove Move{};
};

//������ �����ֱ⸶�� Ŭ���̾�Ʈ���� ��ġ������ Ŭ���̾�Ʈ��� �������ֵ����Ѵ�.
struct BattileFieldTankUpdateSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattileFieldTankUpdateSyn, BATTLE_FIELD_TANK_UPDATE_SYN)

	int Count{};
	TankMove Move[ROOM_MAX_PLAYER_COUNT]{};
};



struct BattleFieldPlayWaitEndSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldPlayWaitEndSyn, BATTLE_FIELD_PLAYWAIT_END_SYN)

	RoomState RoomState;
	float LeftTime;
};

struct BattleFieldPlayingEndSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldPlayingEndSyn, BATTLE_FIELD_PLAYING_END_SYN)

	RoomState RoomState;
	float LeftTime;
	int Count;
	BattleInfo Info[ROOM_MAX_PLAYER_COUNT]{};
	int WinnerCharacetrUID = INVALID_UID;		// ���� UID
};

struct BattleFieldEndWaitEndSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldEndWaitEndSyn, BATTLE_FIELD_ENDWAIT_END_SYN)

	RoomState RoomState;
};


struct BattleFieldLeaveSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldLeaveSyn, BATTLE_FIELD_LEAVE_SYN);
};

struct BattleFieldLeaveAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldLeaveAck, BATTLE_FIELD_LEAVE_ACK);
	int CharacterUID = INVALID_UID;		// ���� ������ UID �̰� Ȯ���ؼ� �ʵ��� ��ũ�� ���������ֵ��� ����
										// �ڱ��ڽ��� ���� �ٲ��ֵ��� ����
};

struct ChatMessageSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(ChatMessageSyn, CHAT_MESSAGE_SYN);
	PlayerState PlayerState;
	char Message[MESSAGE_LEN];
};

struct ChatMessageAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(ChatMessageAck, CHAT_MESSAGE_ACK);
	char Message[MESSAGE_LEN];
};

struct BattleFieldFireSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldFireSyn, BATTLE_FIELD_FIRE_SYN);
	BulletInfo BulletInfo;
};

struct BattleFieldFireAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldFireAck, BATTLE_FIELD_FIRE_ACK);
	BulletInfo BulletInfo;
};


struct BattleFieldDeathSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldDeathSyn, BATTLE_FIELD_DEATH_SYN);
	int CharacterUID = INVALID_UID;		// �������� �׾�����
};

struct BattleFieldDeathAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldDeathAck, BATTLE_FIELD_DEATH_ACK);
	int CharacterUID = INVALID_UID;		// ���� �׾�����
	float RevivalLeftTime;				// ��Ȱ���� ���� �ð�
};

struct BattleFieldRevivalSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldRevivalSyn, BATTLE_FIELD_REVIVAL_SYN);
	int CharacterUID = INVALID_UID;		// ���� ��Ȱ�ߴ���
	TankMove RevivalMove;						// ��Ȱ ��ġ
};

struct BattleFieldStatisticsUpdateSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(BattleFieldStatisticsUpdateSyn, BATTLE_FILED_STATISTICS_UPDATE_SYN);

	int Count;
	BattleInfo Info[ROOM_MAX_PLAYER_COUNT];
};



// ���� �ֱ⸶�� Ŭ�� TCP ������ �ð��� �����Ѵ�.
// ������ �̸� �����ϰ� ������ �ð��� Ŭ���̾�Ʈ ������ ����Ѵ�.
// ������ ���������� Ŭ���̾�Ʈ�� �ð��� üũ�ϰ� �������� �ð����� �ʴ� ��� ������ ������ ����������� �Ѵ�.
// �̷��� ���� �÷��̾ ����°��� ���� �� ���� �� ������ �� ȥ��������
struct TcpRTTSyn : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpRTTSyn, TCP_RTT_SYN)
	Int64U Tick;			// Ŭ���̾�Ʈ ������ �߼��� �ð��� ����
};

// ���� �����ϸ� ��ٷ� ��󿡰� Ack�� �������ش�.
// Ŭ��� �̸� �����ϰ� Ŭ���̾�Ʈ�� ����Ѵ�. Ŭ�� ������ �ð����� ACK�� �������� ������ ������ ������ �Ѵ�.
struct TcpRTTAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(TcpRTTAck, TCP_RTT_ACK)
	Int64U Tick;			// �������� ������ �ð��� ��Ƽ� Ŭ��� ����
							// ������ �ð��� �� Tick���� ���̸� ���ؼ� Latency�� ���ϰ�
							// �����������ð��� ������ �ð��� ���ؼ� RTT�� ���� �� �ְڴ�.
};

