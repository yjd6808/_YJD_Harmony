#pragma once

#include <Common/Enum.h>
#include <Common/Structure.h>

#include <JCore/Time.h>
#include <JCore/LockGuard.h>
#include <JNetwork/Host/TcpSession.h>

#include <TF/Game/Channel.h>
#include <TF/Game/Room.h>

#define INVALID_UID		-1

#ifndef Out_
#define Out_
#endif

#ifndef In_
#define In_
#endif

class Player
{
public:
	Player(JNetwork::TcpSession* session) : m_pSession(session) {}

	JNetwork::TcpSession* Session() const { return m_pSession; }
	void Lock()							  { m_PlayerMtx.Lock(); }
	void Unlock()						  { m_PlayerMtx.Unlock(); }

	void SetAccountUID(int accountUID);
	void UpdateLoggedInTime();
	JCore::DateTime GetLoggedInTime();
	int GetAccountUID();
	void SetChannelUID(int channelUID);
	int GetChannelUID();
	void SetCharacterUID(int characterUID);
	int GetCharacterUID();
	void SetPlayerState(PlayerState playerState);
	PlayerState GetPlayerState();
	CharacterInfo GetCharacterInfo();
	void UpdateCharacterInfo(const CharacterInfo& info);
	int GetRoomUID();
	void SetRoomUID(int roomUID);
	void SetRoomHost(bool isHost);
	void SetReady(bool ready);
	void LoadCharacterInfo(Out_ CharacterInfo& info);			// 정보를 얻을 때 사용
	void LoadRoomCharacterInfo(Out_ RoomCharacterInfo& info);	// 정보를 얻을 때 사용

	bool CheckNameEqual(const JCore::String& name);
private:
	int m_iAccountUID = INVALID_UID;
	int m_iChanneldUID = INVALID_UID;
	int m_iRoomUID = INVALID_UID;
	bool m_bRoomHost = false;
	bool m_bReady = false;

	CharacterInfo m_CharacterInfo;
	TankMove m_TankState{};
	PlayerState m_ePlayerState = PlayerState::LoginStage;	// 세션 생성시 바로 로그인 씬이므로
	JCore::DateTime m_LoggedInTime;
	JNetwork::TcpSession* m_pSession = nullptr;
	JCore::SpinLock m_PlayerMtx;

	friend class Channel;
	friend class Room;
};


