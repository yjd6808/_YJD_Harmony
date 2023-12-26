/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:22:52 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>

#include <TF/Common/Base/IOpenCloseable.h>
#include <TF/Common/Base/IUpdatable.h>
#include <TF/Common/Base/IBroadcastable.h>

#include <TF/Common/Structure.h>

#include <TF/Server/Contents/Player.h>

class Room : public JCore::AccessibleObjectPool<Room>, public IBroadcastable, public IUpdatable, public IOpenCloseable
{
public:
	Room();

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void OnUpdateBattle(const JCore::TimeSpan& elapsed);
	void OnUpdatePlayWait(const JCore::TimeSpan& elapsed);
	void OnUpdatePlaying(const JCore::TimeSpan& elapsed);
	void OnUpdateEndWait(const JCore::TimeSpan& elapsed);
	void OnRoomStateChanged(RoomState changedState);

	
	void JudgeWinnerLoserRaw();

	void OnBattleBegin();
	void OnBattleEnd();

	void BroadcastPacket(JNetwork::IPacket* packet, int state) override;
	void OnPopped() override;
	void OnPushed() override;

	

	void Close() override;
	void Open() override;
	bool IsClosed() override { return m_bClosed; }
	bool IsOpened() override { return !m_bClosed; }
	bool IsAllReady() const;

	void SetName(const JCore::String& name);
	void SetLobby(ChannelLobby* lobby);
	void SetChannel(Channel* channel);
	void SetNameRaw(const char* name);
	void SetState(RoomState state);
	void SetNameRaw(const JCore::String& name);
	JCore::String GetName();

	int GetPlayerCount();
	int GetPlayerCountRaw() const;
	int GetBattleFieldPlayerCountRaw() const;
	Character* GetBattleTopKillerRaw() const;
	Character* GetCharacterByCharacterPrimaryKey(int characterPrimaryKey) const;
	RoomState GetState() const { return m_eState; }
	JCore::TimeSpan GetBattleFieldTime() { return m_ElapsedBattleTime.Elapsed; }
	void GetRoomInfo(JCORE_REF_OUT RoomInfo& info);
	void GetRoomMemberInfoList(JCORE_OUT RoomCharacterInfo* info);
	JCore::Vector<TankMoveNet> GetRoomMemberLiveMoveList();			// 살아있는 탱크 위치정보
	void GetRoomMemberInfoListRaw(JCORE_OUT RoomCharacterInfo* info);
	Player* GetHostPlayer() const { return m_pLeader; }

	bool IsEmpty();
	bool IsEmptyRaw();

	int Join(Player* player);
	bool Leave(Player* player);

	bool AddPlayer(Player* player);
	int AddPlayerRaw(Player* player);

	bool RemovePlayer(Player* player);
	bool RemovePlayerRaw(Player* player);

	void ChooseNewLeaderRaw();

	void BroadcastRoomMemberListInfo();
	void BroadcastRoomMemberInfo(Character* character);
	void BroadcastRoomInfo();
	void BroadcastRoomGameStart();
	void BroadcastBattleFieldTankSpawn(Character* character, bool revival);
	void BroadcastBattleFieldMove(const TankMoveNet& move);
	void BroadcastBattleFieldDeath(int deadCharacterPrimaryKey);
	void BroadcastBattleSatistics();
	void BroadcastBattleFieldFire(const BulletInfoNet& info);
	void BroadcastBattleStateChanged(RoomState changedState);
	void BroadcastBattleFieldLeave(Player* outPlayer);
	void BroadcastJudgeRaw(int winnerCharacterPrimaryKey);
	void BroadcastRoomGameEnd();
	void BroadcastTimeSync();
	
	


private:
	mutable JCore::RecursiveLock m_Sync;

	Channel* m_pChannel;
	ChannelLobby* m_pLobby;
	RoomState m_eState;
	JCore::String m_szName;
	JCore::AtomicBool m_bClosed;
	JCore::Vector<Player*> m_vPlayerList;
	Player* m_pLeader;

	JCore::TimeCounter m_ElapsedBattleTime;					// 배틀필드 고유 게임시간
	JCore::TimeCounter m_ElapsedBroadcastBattleStatistics;	// 배틀 통계정보 전파시간
};
