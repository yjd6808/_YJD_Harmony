#pragma once

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
	Disconnected	= -1,
	LoginStage		= 0,
	ChannelSelect	= 1,
	CharacterSelect = 2,
	Lobby			= 3,
	RoomReady		= 4,
	RoomPlaying		= 5
};

enum class RoomState
{
	ReadyWait,		// 모든 플레이어가 준비하기를 기다리는 단계
	Playing			// 게임 진입 후
};

enum class ChannelState
{
	Clean,		// 쾌적함
	Busy,		// 혼잡함
	Full		// 꽉참
};