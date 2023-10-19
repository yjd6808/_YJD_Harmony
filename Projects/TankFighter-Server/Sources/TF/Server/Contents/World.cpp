/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:59:56 PM
 * =====================
 *
 */


#include "Pch.h"
#include "World.h"

#include <TF/Common/Command.h>
#include <TF/Server/Query/Q_GAME.h>

USING_NS_JC;

World::World()
{}


void World::Initialize() {
	Qry::SelectChannelInfoResult result = Q_GAME::SelectChannelInfo();

	if (!result.HasBindedResult) {
		_LogError_("%s에서 채널 목록 정보를 얻지 못했습니다.", Core::GameDB->GetName().Source());
		return;
	}

	do {
		Channel* pChannel = dbg_new Channel(
			result.PrimaryKey,
			result.Name,
			result.MaxPlayerCount,
			result.GeneratedTime
		);
		ChannelLobby* pChannelLobby = dbg_new ChannelLobby();

		m_hmChannelMapByPrimaryKey.Insert(result.PrimaryKey, pChannel);
		m_hmLobbyMapByPrimaryKey.Insert(result.PrimaryKey, pChannelLobby);
		m_vChannelList.PushBack(pChannel);
		m_vChannelLobbyList.PushBack(pChannelLobby);
	} while (result.FetchNextRow());
}

void World::Finalize() {
	m_vChannelList.ForEachDelete();
	m_vChannelList.Clear();

	for (int i = 0; i < m_vChannelLobbyList.Size(); ++i) {
		ChannelLobby* pChannelLobby = m_vChannelLobbyList[i];
		pChannelLobby->Finalize();
	}

	m_vChannelLobbyList.ForEachDelete();
	m_vChannelLobbyList.Clear();

}

void World::OnUpdate(const JCore::TimeSpan& elapsed) {
	for (int i = 0; i < m_vChannelList.Size(); ++i) {
		m_vChannelList[i]->OnUpdate(elapsed);
	}
}

void World::BroadcastPacket(JNetwork::ISendPacket* packet, int state) {
	JCORE_REF_COUNT_GUARD(packet);
	JCORE_LOCK_GUARD(m_PlayerListSync);
	m_hsPlayerList.ForEach([=](Player* player) { player->SendPacket(packet); });
}

Vector<ChannelInfo> World::GetChannelInfoList() {
	const int iChannelCount = m_vChannelList.Size();
	Vector<ChannelInfo> info(iChannelCount);
	for (int i = 0; i < iChannelCount; ++i) {
		info.PushBack(m_vChannelList[i]->GetInfo());
	}
	return info;
}

Channel* World::GetChannelByPrimaryKey(int primaryKey) {
	Channel** ppChannel = m_hmChannelMapByPrimaryKey.Find(primaryKey);
	if (ppChannel == nullptr) { return nullptr; }
	return *ppChannel;
}

ChannelLobby* World::GetChannelLobbyByPrimaryKey(int channelPrimaryKey) {
	ChannelLobby** ppChannelLobby = m_hmLobbyMapByPrimaryKey.Find(channelPrimaryKey);
	if (ppChannelLobby == nullptr) { return nullptr; }
	return *ppChannelLobby;
}

void World::AddPlayer(Player* player) {
	{
		JCORE_LOCK_GUARD(m_PlayerListSync);
		m_hsPlayerList.Insert(player);
	}

	{
		JCORE_LOCK_GUARD(m_PlayerMapSync);
		const JCore::String& playerAccountId = player->GetAccountId();
		const bool bSuccess = m_hmPlayerMapByAccountId.Insert(playerAccountId, player);
		if (!bSuccess) {
			m_hmPlayerMapByAccountId[playerAccountId] = player;
		}
	}
}

void World::RemovePlayer(Player* player) {
	{
		JCORE_LOCK_GUARD(m_PlayerListSync);
		m_hsPlayerList.Remove(player);
	}
	{
		JCORE_LOCK_GUARD(m_PlayerMapSync);
		const String& playerAccountId = player->GetAccountId();
		m_hmPlayerMapByAccountId.Remove(playerAccountId);
	}
}

Player* World::GetPlayerByAccountId(const JCore::String& accountId) {
	JCORE_LOCK_GUARD(m_PlayerMapSync);
	Player** ppPlayer = m_hmPlayerMapByAccountId.Find(accountId);
	if (ppPlayer == nullptr) {
		return nullptr;
	}
	return *ppPlayer;
}

