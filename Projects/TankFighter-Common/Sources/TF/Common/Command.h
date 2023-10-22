/*
 * 작성자 : 윤정도
 *
 * 커맨드 구조체 이름 규칙: XX_커맨드명
 * 커맨드 ID 규칙: CMDID_XX_커맨드명
 */

#pragma once

#include <JNetwork/Packet/Command.h>

#include <TF/Common/Structure.h>

// TCP Commands
#define CMDID_CS_LOGIN									100
#define CMDID_SC_LOGIN									101
#define CMDID_CS_REGISTER								102
#define CMDID_SC_REGISTER								103
#define CMDID_CS_LOGOUT									244		// 접속은 유지한채로 로그인화면 이동
#define CMDID_SC_LOGOUT									245		// 정상적으로 로그아웃시 로그인씬으로 이동하도록
#define CMDID_SC_DISCONNECT								206		// 접속을 종료한채로 로그인화면 이동
#define CMDID_CS_LOAD_CHANNEL_INFO						104		// 채널 진입시
#define CMDID_SC_LOAD_CHANNEL_INFO						105
#define CMDID_CS_JOIN_CHANNEL							106		// ChannelLayer에서 채널 버튼 클릭시
#define CMDID_SC_JOIN_CHANNEL							107
#define CMDID_CS_LEAVE_CHANNEL							202		// ChannelLayer에서 채널 버튼 클릭시
#define CMDID_SC_LEAVE_CHANNEL							203		// 서버에서 처리 후 클라한테 알림
#define CMDID_CS_LOAD_CHARACTER_INFO					108		// 캐릭터 선택 진입시
#define CMDID_SC_LOAD_CHARACTER_INFO					109
#define CMDID_CS_CREATE_CHARACTER						110		// CharacterSelectLayer에서 캐릭터 생성 버튼 클릭시 전송
#define CMDID_SC_CREATE_CHARACTER						111
#define CMDID_CS_DELETE_CHARACTER						112		// CharacterSelectLayer에서 캐릭터 제거 버튼 클릭시 전송
#define CMDID_SC_DELETE_CHARACTER						113
#define CMDID_CS_SELECT_CHARACTER_AND_JOIN_LOBBY		114		// CharacterSelectLayer에서 로딩된 캐릭터 버튼 클릭시 전송
#define CMDID_SC_SELECT_CHARACTER_AND_JOIN_LOBBY		115
#define CMDID_CS_JOIN_LOBBY								116		// (로비 진입시) 각종 로비 정보를 요청한다.
#define CMDID_CS_LEAVE_LOBBY							204		// LobbyLayer에서 캐릭터 선택 이동버튼 클릭시
#define CMDID_SC_LEAVE_LOBBY							205		// 서버에서 처리 후 클라한테 알림
#define CMDID_SC_UPDATE_CHARACTER_INFO					117		// 업데이트된 캐릭터 정보를 전송한다.
#define CMDID_SC_UPDATE_ROOMLIST						118		// 업데트된 방 정보들을 전송한다.
#define CMDID_SC_UPDATE_FRIENDLIST						119		// 친구 관계 정보를 전송한다.
#define CMDID_SC_UPDATE_PLAYERLIST						207		// 로비 유저목록 업데이트
#define CMDID_CS_CREATE_ROOM							120		// 방 생성 버튼 클릭시
#define CMDID_SC_CREATE_ROOM							124
#define CMDID_CS_JOIN_ROOM								121		// 방 참가 / 방 목록(리스트뷰)의 방 버튼 클릭시
#define CMDID_SC_JOIN_ROOM								125
#define CMDID_SC_UPDATE_ROOM_MEMBER_LIST				126		// 다른 유저가 방에 참가하거나, 방에 있던 유저가 나가는 경우
#define CMDID_SC_UPDATE_ROOM_MEMBER						225		// 특정 유저 정보 업데이트
#define CMDID_CS_ADD_FRIEND								122		// 친구 추가 버튼 클릭시
#define CMDID_CS_ADD_FRIEND_REQUEST						128		// 친구 요청을 받은 클라이언트가 수락/거부의 결과를 서버로 전송한다.
#define CMDID_SC_ADD_FRIEND_REQUEST						128		// 친구 추가 요청 대상에게 요청정보를 전달한다.
#define CMDID_CS_DELETE_FRIEND							123		// 친구 삭제 / 친구 목록(리스트뷰)의 친구 버튼 클릭시
#define CMDID_CS_LOAD_ROOM_INFO							131		// (방 진입 시) 각종 방 정보를 요청한다.
#define CMDID_SC_LOAD_ROOM_INFO							132
#define CMDID_CS_ROOM_GAME_START						133		// 게임 시작 버튼 클릭
#define CMDID_SC_ROOM_GAME_START						137		// 방 안의 멤버들에게 게임 시작 패킷 전송
#define CMDID_SC_ROOM_GAME_END							210		// 방 안의 멤버들에게 게임 종료 패킷 전송

#define CMDID_CS_ROOM_GAME_INTRUDE						208		// 게임 난입 버튼 클릭
#define CMDID_SC_ROOM_GAME_INTRUDE						209		// 난입 유저에게 알려줌
#define CMDID_CS_ROOM_GAME_READY						134		// 게임 준비 버튼 클릭
#define CMDID_SC_ROOM_GAME_READY						137		// 준비 요청에 대한 응답
#define CMDID_CS_ROOM_GAME_READY_CANCEL					135		// 준비 해제 버튼 클릭
#define CMDID_SC_ROOM_GAME_READY_CANCEL					138		// 준비 해제에 대한 응답
#define CMDID_CS_ROOM_LEAVE								136		// 방 나가기 버튼 클릭
#define CMDID_SC_ROOM_LEAVE								139		// 방 나가기에 대한 응답
#define CMDID_SC_ROOM_GAME_JUDGE						223		// 게임 승/패자 알려줌
#define CMDID_CS_LOAD_BATTLE_FIELD_INFO					150		// 방에 진입하면 서버로 난 준비됐소! 라고 알려준다.
#define CMDID_CS_BATTLE_FIELD_MOVE						161		// 클라에서 움직일때마다 일정 주기마다 서버로 자신의 위치를 전송한다. - 서버는 이를 업데이트한다.
#define CMDID_SC_BATTLE_FIELD_MOVE						162		// 서버는 일정주기마다 플레이어 위치를 플레이중인 방의 유저들에게 브로드캐스팅 해준다.
#define CMDID_CS_BATTLE_FIELD_FIRE						166		// 플레이어 총알 발사 후 위치 정보 전달
#define CMDID_SC_BATTLE_FIELD_FIRE						167		// 총알 발사했다고 브로드캐스팅
#define CMDID_SC_BATTLE_FIELD_STATE_CHANGED				163		// 배틀필드 방 상태가 변경될 경우 전파
#define CMDID_SC_BATTLE_FIELD_TIME_SYNC					164		// 배틀필드 게임 경과시간 동기화
#define CMDID_SC_BATTLE_FILED_STATISTICS_UPDATE			171		// 주기적으로 배틀필드 통계 업데이트
#define CMDID_SC_BATTLE_FIELD_TANK_SPAWN				165		// 탱크 생성시 알려준다.
#define CMDID_CS_BATTLE_FIELD_DEATH						168
#define CMDID_SC_BATTLE_FIELD_DEATH						169		// 다른 플레이어에게 브로드캐스팅 및 본인에 해당하는 경우 리바이벌 타이머 설정을 진행한다.
#define CMDID_CS_SERVER_MESSAGE							400		// 서버 메시지 전송
#define CMDID_CS_CHAT_MESSAGE							401		// 채팅 메시지 전송
#define CMDID_SC_CHAT_MESSAGE							402		// 브로드캐스트 전송
#define CMDID_CS_TCP_RTT								200		// RTT/레이턴시 계산을 위해 현재 시간을 담아서 전달
#define CMDID_SC_TCP_RTT								201






// UDP Commands
#define CMDID_CS_UDP_PING								2000
#define CMDID_SC_UDP_PING								2001

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(CS_Login, CMDID_CS_LOGIN)
JCore::StaticString<Const::Length::Id> Id;
JCore::StaticString<Const::Length::Pass> Password;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_Login, CMDID_SC_LOGIN)
int AccountPrimaryKey = Const::InvalidValue;
bool Result;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_Register, CMDID_CS_REGISTER)
JCore::StaticString<Const::Length::Id> Id;
JCore::StaticString<Const::Length::Pass> Password;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_Register, CMDID_SC_REGISTER)
bool Result;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_Logout, CMDID_CS_LOGOUT)
int AccountPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_Logout, CMDID_SC_LOGOUT)
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_LoadChannelInfo, CMDID_CS_LOAD_CHANNEL_INFO)
int ChannelPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

// 최대 10개의 채널 정보까지 받을 수 있도록.
DYNAMIC_CMD_BEGIN(SC_LoadChannelInfo, CMDID_SC_LOAD_CHANNEL_INFO, ChannelInfo)
ChannelInfo Info[1];
DYNAMIC_CMD_END

STATIC_CMD_BEGIN(CS_JoinChannel, CMDID_CS_JOIN_CHANNEL)
int ChannelPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_JoinChannel, CMDID_SC_JOIN_CHANNEL)
int ChannelPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_LeaveChannel, CMDID_CS_LEAVE_CHANNEL)
int ChannelPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_LeaveChannel, CMDID_SC_LEAVE_CHANNEL)
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_LoadCharacterInfo, CMDID_CS_LOAD_CHARACTER_INFO)
int AccountPrimaryKey = Const::InvalidValue;
int ChannelPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_LoadCharacterInfo, CMDID_SC_LOAD_CHARACTER_INFO, CharacterInfo)
CharacterInfo Info[1];
DYNAMIC_CMD_END

STATIC_CMD_BEGIN(CS_CreateCharacter, CMDID_CS_CREATE_CHARACTER)
int AccountPrimaryKey = Const::InvalidValue;
int ChannelPrimaryKey = Const::InvalidValue;
JCore::StaticString<Const::Length::Name> CharacterName;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_DeleteCharacter, CMDID_CS_DELETE_CHARACTER)
int AccountPrimaryKey = Const::InvalidValue;
int ChannelPrimaryKey = Const::InvalidValue;
JCore::StaticString<Const::Length::Name> CharacterName;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_SelectCharacterAndJoinLobby, CMDID_CS_SELECT_CHARACTER_AND_JOIN_LOBBY)
int AccountPrimaryKey = Const::InvalidValue;
int ChannelPrimaryKey = Const::InvalidValue;
int CharacterPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_SelectCharacterAndJoinLobby, CMDID_SC_SELECT_CHARACTER_AND_JOIN_LOBBY)
CharacterInfo info;
STATIC_CMD_END


STATIC_CMD_BEGIN(CS_LoadLobbyInfo, CMDID_CS_JOIN_LOBBY)
int AccountPrimaryKey = Const::InvalidValue;
int ChannelPrimaryKey = Const::InvalidValue;
int CharacterPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_LeaveLobby, CMDID_CS_LEAVE_LOBBY)
int AccountPrimaryKey = Const::InvalidValue;
int ChannelPrimaryKey = Const::InvalidValue;
int CharacterPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_LeaveLobby, CMDID_SC_LEAVE_LOBBY)
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_UpdateCharacterInfo, CMDID_SC_UPDATE_CHARACTER_INFO)
CharacterInfo Info{};
STATIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_UpdateRoomList, CMDID_SC_UPDATE_ROOMLIST, RoomInfo)
RoomInfo Info[1];
DYNAMIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_UpdateFriendList, CMDID_SC_UPDATE_FRIENDLIST, FriendCharacterInfo)
FriendCharacterInfo Info[1];
DYNAMIC_CMD_END

STATIC_CMD_BEGIN(CS_CreateRoom, CMDID_CS_CREATE_ROOM)
JCore::StaticString<Const::Length::Name> RoomName;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_CreateRoom, CMDID_SC_CREATE_ROOM)
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_JoinRoom, CMDID_CS_JOIN_ROOM)
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END


STATIC_CMD_BEGIN(SC_JoinRoom, CMDID_SC_JOIN_ROOM)
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_UpdateRoomMemberList, CMDID_SC_UPDATE_ROOM_MEMBER_LIST, RoomCharacterInfo)
int HostCharacterPrimaryKey = Const::InvalidValue;		// 방장
RoomCharacterInfo Info[1];
DYNAMIC_CMD_END

STATIC_CMD_BEGIN(SC_UpdateRoomMember, CMDID_SC_UPDATE_ROOM_MEMBER)
RoomCharacterInfo Info;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_BattleFieldTankSpawn, CMDID_SC_BATTLE_FIELD_TANK_SPAWN)
TankMoveNet Move;	// 생성위치
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_AddFriend, CMDID_CS_ADD_FRIEND)
JCore::StaticString<Const::Length::Name> FriendName;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_AddFriendRequest, CMDID_SC_ADD_FRIEND_REQUEST)
CharacterInfo Info;					// 친구 요청을 보낸 사람의 캐릭터 정보
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_AddFriendRequest, CMDID_CS_ADD_FRIEND_REQUEST)
bool Accept = false;				// 친구 요청 수락/거부 여부
int RequestCharacterAccessId;		// 친구 요청한 사람의 ID
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_DeleteFriend, CMDID_CS_DELETE_FRIEND)
int DeleteCharacterPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_ServerMessage, CMDID_CS_SERVER_MESSAGE, JCore::StaticString<1>)
JCore::StaticString<1> Message;
DYNAMIC_CMD_END


STATIC_CMD_BEGIN(CS_LoadRoomInfo, CMDID_CS_LOAD_ROOM_INFO)
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_LoadRoomInfo, CMDID_SC_LOAD_ROOM_INFO)
RoomInfo Info;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_RoomLeave, CMDID_CS_ROOM_LEAVE)
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_RoomLeave, CMDID_SC_ROOM_LEAVE)
STATIC_CMD_END


STATIC_CMD_BEGIN(CS_RoomGameReady, CMDID_CS_ROOM_GAME_READY)
int CharacterPrimaryKey = Const::InvalidValue;
int RoomAccessId = Const::InvalidValue;
bool Ready;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_RoomGameReady, CMDID_CS_ROOM_GAME_READY)
int CharacterPrimaryKey;
bool Ready;
STATIC_CMD_END

// 게임 시작/게임 난입 기능 동시에 처리하도록 함
// 패킷 하나더 추가하면 되는데 귀찮기도 하고 걍 이래처리함
STATIC_CMD_BEGIN(CS_RoomGameStart, CMDID_CS_ROOM_GAME_START)
int CharacterPrimaryKey = Const::InvalidValue;
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_RoomGameStart, CMDID_SC_ROOM_GAME_START)
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_RoomGameEnd, CMDID_SC_ROOM_GAME_END)
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_RoomGameIntrude, CMDID_CS_ROOM_GAME_INTRUDE)
int CharacterPrimaryKey = Const::InvalidValue;
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_RoomGameIntrude, CMDID_SC_ROOM_GAME_INTRUDE)
STATIC_CMD_END


STATIC_CMD_BEGIN(CS_LoadBattleFieldInfo, CMDID_CS_LOAD_BATTLE_FIELD_INFO)
int RoomAccessId = Const::InvalidValue;
STATIC_CMD_END

// 클라는 일정주기마다 서버로 자신의 탱크 정보를 서버로 전송한다.
// 서버는 이 정보를 받아서 업데이트 시켜준다.
STATIC_CMD_BEGIN(CS_BattleFieldMove, CMDID_CS_BATTLE_FIELD_MOVE)
TankMoveNet Move {};
STATIC_CMD_END

//서버는 일정주기마다 클라이언트들의 위치정보를 클라이언트들로 전송해주도록한다.
DYNAMIC_CMD_BEGIN(SC_BattleFieldMove, CMDID_SC_BATTLE_FIELD_MOVE, TankMoveNet)
TankMoveNet Move[1];
DYNAMIC_CMD_END

STATIC_CMD_BEGIN(SC_BattleFieldStateChanged, CMDID_SC_BATTLE_FIELD_STATE_CHANGED)
RoomState State;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_BattleFieldTimeSync, CMDID_SC_BATTLE_FIELD_TIME_SYNC)
JCore::TimeSpan Elapsed;
STATIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_BattleFieldStatisticsUpdate, CMDID_SC_BATTLE_FILED_STATISTICS_UPDATE, BattleStatisticsNet)
BattleStatisticsNet Statistics[1];
DYNAMIC_CMD_END

STATIC_CMD_BEGIN(CS_BattleFieldFire, CMDID_CS_BATTLE_FIELD_FIRE)
BulletInfoNet BulletInfo;
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_BattleFieldFire, CMDID_SC_BATTLE_FIELD_FIRE)
BulletInfoNet BulletInfo;
STATIC_CMD_END

STATIC_CMD_BEGIN(CS_BattleFieldDeath, CMDID_CS_BATTLE_FIELD_DEATH)
int KillerCharacterPrimaryKey = Const::InvalidValue;		// 누구한테 죽었는지
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_BattleFieldDeath, CMDID_SC_BATTLE_FIELD_DEATH)
int DeadCharacterPrimaryKey = Const::InvalidValue;		// 누가 죽었는지
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_RoomGameJudge, CMDID_SC_ROOM_GAME_JUDGE)
int WinnerCharacterPrimaryKey = Const::InvalidValue;
STATIC_CMD_END

DYNAMIC_CMD_BEGIN(CS_ChatMessage, CMDID_CS_CHAT_MESSAGE, JCore::StaticString<1>)
PlayerState PlayerState;
JCore::StaticString<1> Message;
DYNAMIC_CMD_END

DYNAMIC_CMD_BEGIN(SC_ChatMessage, CMDID_SC_CHAT_MESSAGE, JCore::StaticString<1>)
PlayerState PlayerState;
JCore::StaticString<1> Message;
DYNAMIC_CMD_END


STATIC_CMD_BEGIN(SC_Disconnect, CMDID_SC_DISCONNECT)
STATIC_CMD_END

// 일정 주기마다 클라가 TCP 서버로 시간을 전송한다.
// 서버는 이를 수신하고 수신한 시각을 클라이언트 정보에 기록한다.
// 서버는 지속적으로 클라이언트의 시간을 체크하고 핑전송이 시간내로 않는 경우 강제로 연결을 끊어버리도록 한다.
// 이러면 유령 플레이어가 생기는것을 막을 수 있을 듯 서버도 덜 혼잡해지고
STATIC_CMD_BEGIN(CS_TcpRTT, CMDID_CS_TCP_RTT)
Int64U Tick;			// 클라이언트 측에서 발송한 시각을 전달
STATIC_CMD_END

// 핑을 수신하면 곧바로 SC_대상에게 를 전송해준다.
// 클라는 이를 수신하고 클라이언트에 기록한다. 클라도 정해진 시간내로 ACK를 수신하지 않으면 연결을 끊도록 한다.
STATIC_CMD_BEGIN(SC_TcpRTT, CMDID_SC_TCP_RTT)
Int64U Tick;			// 서버에서 수신한 시각을 담아서 클라로 전달
						// 전달한 시각과 이 Tick과의 차이를 비교해서 Latency를 구하고
						// 이전에보낸시각과 수신한 시각을 비교해서 RTT를 구할 수 있겠다.
STATIC_CMD_END


DYNAMIC_CMD_BEGIN(SC_UpdatePlayerList, CMDID_SC_UPDATE_PLAYERLIST, CharacterInfo)
PlayerState State;		// 이 State인 경우 업데이트
CharacterInfo Info[1];
DYNAMIC_CMD_END

#pragma pack(pop)

