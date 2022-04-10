/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <Common/Enum.h>

#define INVALID_UID				-1

#define ID_LEN					40
#define PASS_LEN				40
#define NAME_LEN				40

#ifndef REASON_LEN
#define REASON_LEN				150
#endif

#ifndef MESSAGE_LEN
#define MESSAGE_LEN				300
#endif

// 가변 커맨드 전달 구현해야하는데 시간이 부족해서 일단 이렇게 만든다.
#ifndef COMMAND_ARRAY_LEN
#define COMMAND_ARRAY_LEN		10
#endif

#define ROOM_MAX_PLAYER_COUNT	4


struct TankMove
{
	int CharacterUID = INVALID_UID;
	float X;
	float Y;
	float MoveSpeed;
	MoveDirection MoveDir;
	
	RotateDirection RotationDir;
	float Rotation;
	float RotationSpeed;
};



struct CharacterInfo
{
	int CharacterUID = INVALID_UID;
	char Name[NAME_LEN];
	int Win;
	int Lose;
	int Kill;
	int Death;
	int Money;
	PlayerState PlayerState;
};

struct RoomCharacterInfo : CharacterInfo
{
	int RoomUID = INVALID_UID;
	bool Ready;
	bool IsDeath;
};

struct ChannelInfo
{
	Int8 ChannelUID = INVALID_UID;
	int PlayerCount;
	int MaxPlayerCount;
	char Name[NAME_LEN];
};

struct RoomInfo
{
	char Name[NAME_LEN];
	int RoomUID = INVALID_UID;
	int PlayerCount;
	int MaxPlayerCount;
	RoomState RoomState;

	bool IsBattleEndingState() const {
		return RoomState >= RoomState::EndWait;
	}

	bool IsBattlePlayingState() const {
		return RoomState >= RoomState::PlayWait && RoomState <= RoomState::Playing;
	}

	bool IsLobbyState() const {
		return RoomState >= RoomState::ReadyWait;
	}
};

struct BattleInfo
{
	int CharacterUID = INVALID_UID;
	int Kill;
	int Death;
	int FireCount;
};

struct BulletInfo
{
	int CharacterUID = INVALID_UID;		// 발사한놈
	float X;
	float Y;
	float Rotation;
	float Diameter;
	float MoveSpeed;
	Int32U Color;
};