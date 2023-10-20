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
	int AccessId = Const::InvalidValue;
	int PrimaryKey = Const::InvalidValue;
	JCore::StaticString<Const::Length::Name> Name;
	int Win{};
	int Lose{};
	int Kill{};
	int Death{};
	int Money{};
	bool LoggedIn{};
};

struct FriendCharacterInfo
{
	int PrimaryKey;
	JCore::StaticString<Const::Length::Name> Name;
	bool LoggedIn;
};

struct RoomCharacterInfo : CharacterInfo
{
	bool Ready;
	bool IsDeath;
};

struct ChannelInfo
{
	int PrimaryKey = Const::InvalidValue;
	int PlayerCount;
	int MaxPlayerCount;
	JCore::StaticString<Const::Length::Name> Name;
};

struct RoomInfo
{
	JCore::StaticString<Const::Length::Name> Name;
	int AccessId = Const::InvalidValue;
	int PlayerCount = 0;
	int MaxPlayerCount = 0;
	RoomState RoomState = RoomState::ReadyWait;

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