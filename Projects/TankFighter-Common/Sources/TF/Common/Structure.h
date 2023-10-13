/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>

#include <TF/Common/Enum.h>
#include <TF/Common/Const.h>

 // disable all warnings
#pragma warning(push, 0)

struct TankMove
{
	int CharacterUID = Const::InvalidValue;
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
	int CharacterUID = Const::InvalidValue;
	char Name[Const::Length::Name];
	int Win;
	int Lose;
	int Kill;
	int Death;
	int Money;
	PlayerState PlayerState;
};

struct RoomCharacterInfo : CharacterInfo
{
	int RoomUID = Const::InvalidValue;
	bool Ready;
	bool IsDeath;
};

struct ChannelInfo
{
	Int8 ChannelUID = Const::InvalidValue;
	int PlayerCount;
	int MaxPlayerCount;
	char Name[Const::Length::Name];
};

struct RoomInfo
{
	char Name[Const::Length::Name];
	int RoomUID = Const::InvalidValue;
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
	int CharacterUID = Const::InvalidValue;
	int Kill;
	int Death;
	int FireCount;
};

struct BulletInfo
{
	int CharacterUID = Const::InvalidValue;		// 발사한놈
	float X;
	float Y;
	float Rotation;
	float Diameter;
	float MoveSpeed;
	Int32U Color;
};

#pragma warning(pop)