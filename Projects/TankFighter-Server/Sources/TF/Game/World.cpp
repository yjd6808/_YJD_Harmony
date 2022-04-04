#define _WINSOCKAPI_		// winsock.h 연속 include 방지


#include <TF/Game/World.h>
#include <TF/Game/Channel.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Database/MysqlQueryFuture.h>
#include <TF/Util/Console.h>


using namespace JCore;

World* World::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new World();
	}

	return ms_pInstance;
}


bool World::Initialize() {
	
	auto spChannelInfos = MysqlDatabase::GetInstance()->Query("select * from t_channel");

	for (int i = 1; i <= spChannelInfos->GetResultRowCount(); i++) {
		int iChannedUID = spChannelInfos->GetInt(i, 0);
		String szChannelName = spChannelInfos->GetString(i, 1);
		int iMaxPlayerCount = spChannelInfos->GetInt(i, 2);

		m_ChannelMap.Insert(iChannedUID, new Channel(iChannedUID, szChannelName, iMaxPlayerCount));
		Console::WriteLine(ConsoleColor::LIGHTCYAN, "%d 채널 초기화 완료", iChannedUID);
	}

	return true;
}

bool World::Finalize() {
	return true;
}

int World::GetTotalRoomCount() {
	return 1;
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
			DebugAssert(false, "플레이어가 속한 채널 UID가 이상합니다.");
			return false;
		}

		if (!m_ChannelMap[iChannedUID]->RemovePlayer(player)) {
			DebugAssert(false, "플레이어가 속한 채널에 없습니다.");
			return false;
		}
	}

	return m_PlayerMap.RemovePlayer(player->GetAccountUID());
}

Channel* World::GetChannel(int channelUID) {
	if (!m_ChannelMap.Exist(channelUID))
		return nullptr;

	return m_ChannelMap[channelUID];
}
