/*
 * 작성자: 윤정도
 * 생성일: 10/16/2023 6:33:22 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Pool/AccessibleObjectPool.h>
#include <JCore/Primitives/String.h>

#include <TF/Common/Base/IUpdatable.h>
#include <mutex>

struct CharacterInfo;
class Player;

class Character : public JCore::AccessibleObjectPool<Character>
{
public:
	Character();
	~Character() override;

	void OnPopped() override;
	void OnPushed() override;

	void OnUpdate(const JCore::TimeSpan& elapsed);
	void UpdateRevivalTime(const JCore::TimeSpan& elapsed);

	Player* GetPlayer() const { return m_pPlayer; }
	int GetPrimaryKey() const { return m_iPrimaryKey; }
	const JCore::String& GetName() const { return m_szName; }
	int GetWinCount() const { return m_iWin; }
	int GetLoseCount() const { return m_iLose; }
	int GetKillCount() const { return m_iKill; }
	int GetBattleKillCount() const { return m_BattleStatistics.Kill; }
	int GetBattleDeathCount() const { return m_BattleStatistics.Death; }
	JCore::DateTime GetBattleLastKillTime() const { return m_BattleStatistics.LastKillTime; }
	bool IsDeath() const { return m_iDeath; }
	bool IsReady() const { return m_bReady; }

	void ClearBattleInfo();

	void SetPlayer(Player* player) { m_pPlayer = player; }
	void SetRoom(Room* room) { m_pRoom = room; }
	void SetDeath(bool death);
	void SetReady(bool ready);
	void SetRevivalTime(Int64 tick);

	void AddWinCount(int win, bool dirty = true);
	void AddLoseCount(int lose, bool dirty = true);
	void AddDeathCount(int death, bool dirty = true);
	void AddKillCount(int kill, bool dirty = true);
	void AddBattleFireCount(int count) { m_BattleStatistics.FireCount += count; }
	void AddBattleDeathCount(int count) { m_BattleStatistics.Death += count; }
	void AddBattleKillCount(int count);

	void ApplyBattleStatisticsToInfo();
	void ApplyToDatabase();

	void LoadFriendList(ChannelLobby* channelLobby);
	int GetFriendCount() const;
	void GetFriendList(JCORE_OUT FriendCharacterInfo* friendArray, int capacity);
	bool IsFriend(int characterPrimaryKey);
	void AddFriend(Character* friendCharactor);
	void DeleteFriend(int characterPrimaryKey);
	void ChangeFriendState(int friendCharacterPrimaryKey, bool login);
	void NotifyLoginStateToFriends(bool login);
	
	void SetInfo(const CharacterInfo& info, bool dirty = true);
	void SetMove(const TankMove& move);

	void GetInfo(JCORE_REF_OUT CharacterInfo& info);
	void GetRoomInfo(JCORE_REF_OUT RoomCharacterInfo& info);
	void GetFriendInfo(JCORE_REF_OUT FriendCharacterInfo& info);
	void GetBattleStatisticsNet(JCORE_REF_OUT BattleStatisticsNet& info);
	void GetMoveNet(JCORE_REF_OUT TankMoveNet& move);

	JCore::String ToString();
	
private:
	Player* m_pPlayer;
	Room* m_pRoom;

	mutable JCore::NormalLock m_FriendLock;
	JCore::Vector<FriendCharacterInfo> m_vFriendList;

	// 초기 생성후 변경안되는 값들
	int m_iPrimaryKey;		
	JCore::String m_szName;

	// 외부 쓰레드에서 변경될 수 있으므로 아토믹으로 둠
	JCore::AtomicInt m_iWin;
	JCore::AtomicInt m_iLose;
	JCore::AtomicInt m_iDeath;
	JCore::AtomicInt m_iKill;
	JCore::AtomicInt m_iMoney;

	// 배틀필드관련
	BattleStatistics m_BattleStatistics;
	TankMove m_Move;

	JCore::TimeSpan m_RevivalTime;
	bool m_bReady;				// 방 레디 여부
	bool m_bDeath;				// 사망여부
	bool m_bDirty;
};
