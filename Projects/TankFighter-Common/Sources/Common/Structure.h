#pragma once

#include <JCore/Type.h>

#define INVALID_UID				-1

#define ID_LEN					40
#define PASS_LEN				40
#define NAME_LEN				40

#ifndef REASON_LEN
#define REASON_LEN				150
#endif


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
	int UID = INVALID_UID;
	char Name[NAME_LEN];
	int Win;
	int Lose;
	int Kill;
	int Death;
	int Money;
};

struct ChannelInfo
{
	Int8 UID = INVALID_UID;
	int PlayerCount;
	int MaxPlayerCount;
	char Name[NAME_LEN];
};

struct RoomInfo
{
	int PlayerCount;
	int MaxPlayerCount;
	int RoomState;
	int MapUID;
};