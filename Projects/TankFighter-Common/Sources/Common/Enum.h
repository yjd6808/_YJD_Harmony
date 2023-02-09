/*0
 * 작성자 : 윤정도
 */

#pragma once

enum class SceneType
{
	Login,
	Channel,
	CharacterSelect,
	Lobby,
	Room,
	BattleField
};

enum class MoveDirection
{
	None,
	Forward,
	Backward
};

enum class RotateDirection
{
	None,
	Right,
	Left
};

enum class ObstacleShape
{
	None,
	Rectangle,			// 사각형
	Circle,				// 원형
	Rhombus				// 마름모
};

enum class PlayerState
{
	Disconnected	= -1,		// 연결 끊긴 상태 혹은 초기화된 상태
	LoginStage		= 0,		// 로그인 스테이지
	ChannelSelect	= 1,		// 채널 선택 스테이지
	CharacterSelect = 2,		// 캐릭터 선택 스테이지
	Lobby			= 3,		// 채널 로비
	RoomLobby		= 4,		// 방 로비에 있는 상태
	RoomBattle		= 5			// 방에서 게임을 시작한 상태
};

enum class RoomState
{
	ReadyWait,		// 방 안의 모든 플레이어가 준비하기를 기다리는 단계
	PlayWait,		// 방 안의 모든 플레이어가 배틀필드 진입을 기다리는 단계 - 방 배틀필드 타이머 5초 동작 - 5초내로 모든 플레이어 진입안하면 그냥 시작
					// 5초 지나면 게임 진행 시작 - 모든 플레이어에게 게임 진행시작을 알리고 Playing 상태로 이동 및 타이머 세팅
	Playing,		// 게임 진입 후
	EndWait			// 게임시간이 모두 만료되고 최종 결과를 보여주는 단계 타이머 10초동안 세팅하고 다되면 모두 로비로 이동시킴
};

enum class ChannelState
{
	Clean,		// 쾌적함
	Busy,		// 혼잡함
	Full		// 꽉참
};