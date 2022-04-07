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
#define UPDATE_FRIENDLIST_ACK			119		// 서버 -> 클라 / 친구 관계 정보를 전송한다.

#define CREATE_ROOM_SYN					120		// 클라 -> 서버 / 방 생성 버튼 클릭시
#define CREATE_ROOM_ACK					124		// 서버 -> 클라

#define JOIN_ROOM_SYN					121		// 클라 -> 서버 / 방 참가 / 방 목록(리스트뷰)의 방 버튼 클릭시
#define JOIN_ROOM_ACK					125		// 서버 -> 클라

#define UPDATE_ROOMINFO_ACK				126		// 서버 -> 클라 / 다른 유저가 방에 참가하거나, 방에 있던 유저가 나가는 경우

#define ADD_FRIEND_SYN					122		// 클라 -> 서버 / 친구 추가 버튼 클릭시
#define ADD_FRIEND_ACK					127		// 서버 -> 클라 / 친구 추가 요청에 대한 결과를 보낸다.
#define ADD_FRIEND_REQUEST_SYN			128		// 서버 -> 클라 / 친구 추가 요청 대상에게 요청정보를 전달한다.
#define ADD_FRIEND_REQUEST_ACK			128		// 클라 -> 서버 / 친구 요청을 받은 클라이언트가 수락/거부의 결과를 서버로 전송한다.
// #define ADD_FRIEND_REQUESTR_RESULT_SYN  129	// 서버 -> 클라 / 친구 요청자에게 요청 결과를 전송한다. - 필요 없음


#define DELETE_FRIEND_SYN				123		// 클라 -> 서버 / 친구 삭제 / 친구 목록(리스트뷰)의 친구 버튼 클릭시
#define DELETE_FRIEND_ACK				130		// 서버 -> 클라 / 삭제된 대상이 접속중인 경우 대상에게도 삭제되었다고 송신하고 삭제한 사람에게도 송신한다. 그리고 친구 목록도 갱신해줌

#define LOAD_ROOM_INFO_SYN				131		// 클라 -> 서버 / (방 진입 시) 각종 방 정보를 요청한다.
#define LOAD_ROOM_INFO_ACK				132		// 서버 -> 클라



#define ROOM_GAME_START_SYN				133		// 클라 -> 서버 / 게임 시작 버튼 클릭
#define ROOM_GAME_START_ACK				137		// 서버 -> 클라 / 방 안의 멤버들에게 게임 시작 패킷 전송

#define ROOM_GAME_READY_SYN				134		// 클라 -> 서버 / 게임 준비 버튼 클릭
#define ROOM_GAME_READY_ACK				137		// 서버 -> 클라 / 준비 요청에 대한 응답

#define ROOM_GAME_READY_CANCEL_SYN		135		// 클라 -> 서버 / 준비 해제 버튼 클릭
#define ROOM_GAME_READY_CANCEL_ACK		138		// 서버 -> 클라 / 준비 해제에 대한 응답

#define ROOM_LEAVE_SYN					136		// 클라 -> 서버 / 방 나가기 버튼 클릭
#define ROOM_LEAVE_ACK					139		// 서버 -> 클라 / 방 나가기에 대한 응답

#define SERVER_MESSAGE_SYN				400		// 서버 -> 클라로 특정 메시지 전송

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
	int HostCharacterUID = INVALID_UID;		// 방장
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

	int RequestCharacterUID;	// 친구 요청을 보낸 사람의 UID
	CharacterInfo Info;
};

struct AddFriendRequestAck : JNetwork::ICommand
{
	CMD_DEFAULT_CONSTRUCTOR(AddFriendRequestAck, ADD_FRIEND_REQUEST_ACK)

	bool Accept = false;				// 친구 요청 수락/거부 여부
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

	
	bool Result = false;				// 친구 삭제 성공/실패 여부
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