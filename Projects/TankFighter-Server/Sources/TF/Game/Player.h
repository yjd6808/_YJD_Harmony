/*
 * 작성자 : 윤정도
 */

#pragma once

#include <Common/Enum.h>
#include <Common/Structure.h>

#include <JCore/Time.h>
#include <JCore/Sync/SpinLock.h>
#include <JNetwork/Host/TcpSession.h>

#include <TF/Game/Channel.h>
#include <TF/Game/Room.h>

class Player
{
	Player() {}
public:
	Player(JNetwork::TcpSession* session) : m_pSession(session) {}

	JNetwork::TcpSession* Session() const { return m_pSession; }
	void Lock()							  { m_PlayerMtx.Lock(); }
	void Unlock()						  { m_PlayerMtx.Unlock(); }

	bool SendAsync(JNetwork::ISendPacket* packet);
	bool Disconnect();

	void Initialize();
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
	void UpdateCharacterInfo(const CharacterInfo& info);
	int GetRoomUID();
	void SetRoomUID(int roomUID);
	void SetRoomHost(bool isHost);
	void SetReady(bool ready);
	bool IsReady();
	void LoadCharacterInfo(Out_ CharacterInfo& info);			// 정보를 얻을 때 사용
	void LoadRoomCharacterInfo(Out_ RoomCharacterInfo& info);	// 정보를 얻을 때 사용
	bool CheckNameEqual(const JCore::String& name);

	void LoadBattleInfo(Out_ BattleInfo& battleInfo);
	void AddBattleKillCount(int killCount);
	void AddBattleDeathCount(int deathCount);
	void AddFireCount(int fireCount);
	void InitializeBattleInfo();
	void UpdateTankMove(TankMove& move);
	void LoadMoveInfo(Out_ TankMove& move);
	bool IsDeath();
	void SetDeath(bool isDeath);
	bool IsBattleState();

	void InitializeRoomLobbyState(int roomUID);
	void InitializeRoomBattleState();
	void InitializeRoomBattleIntruderState();

	int SetRevivalLeftTime(int time);
	int GetRevivalLeftTime();

	// 락을 안한 함수 (외부에서 락을 수행한 경우)
	void UnsafeInitializeBattleInfo();
	int UnsafeGetRoomUID() const;
	void UnsafeSetRoomUID(int roomUID);
	void UnsafeSetReady(bool ready);
	bool UnsafeGetReady() const;
	void UnsafeInitializeTankMove();
	void UnsafeSetPlayerState(PlayerState state);
	PlayerState UnsafeGetPlayerState();
	bool UnsafeIsDeath() const;
	void SetLatency(JCore::TimeSpan latency);
	JCore::TimeSpan GetLatency();
private:
	int m_iAccountUID = INVALID_UID;
	int m_iChanneldUID = INVALID_UID;
	bool m_bRoomHost = false;
	int m_iRevivalLeftTime = 0;

	RoomCharacterInfo m_CharacterInfo;
	TankMove m_TankMove{};
	BattleInfo m_BattleInfo;
	JCore::TimeSpan m_Latency;
	JNetwork::TcpSession* m_pSession = nullptr;
	JCore::DateTime m_LoggedInTime;
	JCore::SpinLock m_PlayerMtx;

	friend class Channel;
	friend class Room;
	friend class PlayerPool;
};


