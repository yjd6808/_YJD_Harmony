/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:01:26 PM
 * =====================
 *
 */


#include "Pch.h"
#include "Channel.h"

#include <TF/Server/Const.h>
#include <TF/Common/Command.h>

USING_NS_JC;
USING_NS_JNET;

Channel::Channel()
	: m_iPrimaryKey(-1)
	, m_szName("Empty")
	, m_iMaxPlayerCount(0)
	, m_dtGeneratedTime(0)
	, m_bClosed(false)
{}

Channel::Channel(int uid, String name, int maxPlayerCount, DateTime generatedTime)
	: m_iPrimaryKey(uid)
	, m_szName(name)
	, m_iMaxPlayerCount(maxPlayerCount)
	, m_dtGeneratedTime(generatedTime)
	, m_hsPlayerSet(maxPlayerCount)
	, m_bClosed(false)
{}

ChannelInfo Channel::GetInfo() {
	ChannelInfo info;
	info.Name = m_szName;
	info.MaxPlayerCount = m_iMaxPlayerCount;
	info.PrimaryKey = m_iPrimaryKey;

	{
		JCORE_LOCK_GUARD(m_PlayerListLock);
		info.PlayerCount = m_hsPlayerSet.Size();
	}
	

	return info;
}

int Channel::Join(Player* player) {
	DebugAssert(player);
	const int iAddResult = AddPlayer(player);
	if (iAddResult == Const::Success) {
		player->OnChannelJoin(this);
	}
	return iAddResult;
}

bool Channel::Leave(Player* player) {
	DebugAssert(player);
	const bool bRemoved = RemovePlayer(player);
	if (bRemoved) {
		player->OnChannelLeave();
	}
	return bRemoved;
}

int Channel::AddPlayer(Player* player) {
	if (m_bClosed) 
		return Const::Failed::Channel::AddFailedClosed;

	DebugAssert(player);
	JCORE_LOCK_GUARD(m_PlayerListLock);

	if (m_hsPlayerSet.Size() >= m_iMaxPlayerCount) {
		return Const::Failed::Channel::AddFailedFull;
	}

	const bool bAdded = m_hsPlayerSet.Insert(player);
	if (!bAdded) {
		return Const::Failed::Channel::AddFailedAlreadyExist;
	}

	return Const::Success;
}

bool Channel::RemovePlayer(Player* player) {
	if (player == nullptr)
		return false;

	bool bRemoved;
	{
		JCORE_LOCK_GUARD(m_PlayerListLock);
		bRemoved = m_hsPlayerSet.Remove(player);
	}

	return bRemoved;
}

String Channel::ToString() const {
	return StringUtil::Format("%s 채널", m_szName.Source());
}

void Channel::Close() {
	m_bClosed = true;
}

void Channel::Open() {
	m_bClosed = false;
}

void Channel::OnUpdate(const TimeSpan& elapsed) {
	static Vector<Player*> s_vUpdatesPlayers;
	static auto fnUpdate = [&elapsed](Player* p) { p->OnUpdate(elapsed); };
	

	// 플레이어 업데이트와 플레이어 추가/삭제 동기화를 분리하기 위함.
	// 업데이트 시간이 오래 걸릴경우 채널에 플레이어를 추가/삭제시 병목이 생길 우려가 있기 떄문
	{
		JCORE_LOCK_GUARD(m_PlayerListLock);
		s_vUpdatesPlayers.PushBackAll(m_hsPlayerSet);
	}

	s_vUpdatesPlayers.ForEach(fnUpdate);
	s_vUpdatesPlayers.Clear();
}

void Channel::BroadcastPacket(ISendPacket* packet, int state) {
	static auto fnBroadcast = [&](Player* player) { player->SendPacket(packet); };
	JCORE_REF_COUNT_GUARD(packet);
	JCORE_LOCK_GUARD(m_PlayerListLock);
	m_hsPlayerSet.ForEach(fnBroadcast);
}
