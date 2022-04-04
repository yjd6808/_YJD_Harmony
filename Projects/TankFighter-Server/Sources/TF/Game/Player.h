#pragma once

#include <Common/Enum.h>
#include <Common/Structure.h>

#include <JCore/Time.h>
#include <JCore/LockGuard.h>
#include <JNetwork/Host/TcpSession.h>

#define INVALID_UID		-1

class Player
{
public:
	Player(JNetwork::TcpSession* session) : m_pSession(session) {}

	JNetwork::TcpSession* Session() const { return m_pSession; }

	void SetAccountUID(int accountUID)					{ m_iAccountUID = accountUID; }
	void UpdateLoggedInTime()							{ m_LoggedInTime = JCore::DateTime::Now(); };
	JCore::DateTime GetLoggedInTime() const				{ return m_LoggedInTime; }
	int GetAccountUID() const							{ return m_iAccountUID; }
	void SetChannelUID(int channedUID)					{ m_iChanneldUID = channedUID; }
	int GetChannelUID() const							{ return m_iChanneldUID; }
private:
	int m_iAccountUID = INVALID_UID;
	int m_iChanneldUID = INVALID_UID;
	TankMove m_TankState{};
	PlayerState m_ePlayerState = PlayerState::LoginStage;	// 생성시 바로 로그인 씬이므로
	JCore::DateTime m_LoggedInTime;
	JNetwork::TcpSession* m_pSession = nullptr;
	JCore::CriticalSectionMutex m_PlayerMtx;
};


