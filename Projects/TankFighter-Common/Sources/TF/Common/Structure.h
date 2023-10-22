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
	float X;
	float Y;
	float MoveSpeed;
	MoveDirection MoveDir;
	RotateDirection RotationDir;
	float Rotation;
	float RotationSpeed;
};

struct TankMoveNet : TankMove
{
	int CharacterPrimaryKey = Const::InvalidValue;
};

struct CharacterInfo
{
	int AccessId = Const::InvalidValue;
	int PrimaryKey = Const::InvalidValue;
	JCore::StaticString<Const::Length::Name> Name;
	int WinCount{};
	int LoseCount{};
	int KillCount{};
	int DeathCount{};
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
	bool IsReady;
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

struct BattleStatistics
{
	int Kill;
	int Death;
	int FireCount;
	JCore::DateTime LastKillTime;
};

struct BattleStatisticsNet : BattleStatistics
{
	int CharacterPrimaryKey = Const::InvalidValue;
};

struct BulletInfo
{
	float X;
	float Y;
	float Rotation;
	float Diameter;
	float MoveSpeed;
	Int32U Color;
};

struct BulletInfoNet : BulletInfo
{
	int CharacterPrimaryKey = Const::InvalidValue;		// 발사한놈
};

#pragma warning(pop)