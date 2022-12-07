/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Game/World.h>
#include <TF/Game/Channel.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Database/MysqlQueryFuture.h>
#include <JCore/Utils/Console.h>


using namespace JCore;

World* World::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new World();
	}

	return ms_pInstance;
}


bool World::Initialize() {

	const auto spChannelInfos = MysqlDatabase::GetInstance()->Query("select * from t_channel");

	for (int i = 0; i < spChannelInfos->GetResultRowCount(); i++) {
		int iChannedUID = spChannelInfos->GetInt(i, 0);
		String szChannelName = spChannelInfos->GetString(i, 1);
		const int iMaxPlayerCount = spChannelInfos->GetInt(i, 2);


		Channel* pChannel = new Channel(iChannedUID, szChannelName, iMaxPlayerCount);

		if (!pChannel->Initialize()) {
			SafeConsole::WriteLine(ConsoleColor::Brown, "%d 채널 초기화 중 오류 발생!", iChannedUID);
			return false;
		}

		m_ChannelMap.Insert(iChannedUID, pChannel);
		SafeConsole::WriteLine(ConsoleColor::LightCyan, "%d 채널 초기화 완료", iChannedUID);
	}

	return true;
}

bool World::Finalize() {

	m_ChannelMap.Values().Extension().ForEach([](Channel* channel) {
		channel->Finalize();
		delete channel;
	});

	return true;
}

int World::GetTotalRoomCount() {
	int iTotalRoomCount = 0;
	m_ChannelMap.Values().Extension().ForEach([&iTotalRoomCount](Channel* channel) {
		iTotalRoomCount += channel->GetRoomCount();
	});

	return iTotalRoomCount;
}

int World::GetTotalPlayingRoomCount() {
	int iTotalPlayingRoomCount = 0;
	m_ChannelMap.Values().Extension().ForEach([&iTotalPlayingRoomCount](Channel* channel) {
		iTotalPlayingRoomCount += channel->GetPlayingRoomCount();
	});

	return iTotalPlayingRoomCount;
}

int World::GetTotalPlayerCount() {
	return m_PlayerMap.Count();
}

int World::GetTotalChannelCount() const {
	return m_ChannelMap.Size();
}

bool World::IsPlayerExist(int accountUID) {
	return m_PlayerMap.IsPlayerExist(accountUID);
}

void World::AddNewPlayer(Player* player) {
	m_PlayerMap.AddNewPlayer(player->GetAccountUID(), player);
}

bool World::RemovePlayer(Player* player) {


	if (player->GetChannelUID() != INVALID_UID) {
		const int iChannedUID = player->GetChannelUID();

		if (!m_ChannelMap.Exist(iChannedUID)) {
			DebugAssertMessage(false, "플레이어가 속한 채널 UID가 이상합니다.");
			return false;
		}

		if (!m_ChannelMap[iChannedUID]->RemovePlayer(player)) {
			DebugAssertMessage(false, "플레이어가 속한 채널에 없습니다.");
			return false;
		}
	}

	return m_PlayerMap.RemovePlayer(player->GetAccountUID());
}

Player* World::FindIfPlayer(Func<bool, Player*> predicate) {
	return m_PlayerMap.FindIf(predicate);
}

Channel* World::GetChannel(int channelUID) {
	if (!m_ChannelMap.Exist(channelUID))
		return nullptr;

	return m_ChannelMap[channelUID];
}

Room* World::GetRoomByPlayer(Player* player) {
	Channel* pChannel = GetChannel(player->GetChannelUID());
	if (pChannel == nullptr) {
		return nullptr;
	}

	return pChannel->GetRoomByPlayer(player);
}
