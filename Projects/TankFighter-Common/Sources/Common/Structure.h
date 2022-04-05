#pragma once

#include <JCore/Type.h>

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
	float X;
	float Y;
	float MoveSpeed;
	Int8 MoveDir;

	Int8 RotationDir;
	float Rotation;
	float RotationSpeed;
};

struct BulletMove
{
	float X;
	float Y;
	float Rotation;
};

struct BulletInfo
{
	int BulletType;
};

struct CharacterInfo
{
	int CharacterUID = INVALID_UID;
	int RoomUID = INVALID_UID;
	char Name[NAME_LEN];
	int Win;
	int Lose;
	int Kill;
	int Death;
	int Money;
	int PlayerState;
};

struct RoomCharacterInfo : CharacterInfo
{
	bool Ready;
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
};