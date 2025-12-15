/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:58:45 AM
 * =====================
 *
 */

#include "Core.h"
#include "ServerProcessInfo.h"

//////////////////////////////////////////////////////////////////////////////////////////
ServerProcessInfo* ServerProcessInfoPackage::GetServerProcessInfo(int _serverId)
{
	if (auth_.serverId_ == _serverId)
		return &auth_;

	if (lobby_.serverId_ == _serverId)
		return &lobby_;

	if (center_.serverId_ == _serverId)
		return &center_;

	for (int serverIndex = 0; serverIndex < gameServerList_.Size(); ++serverIndex)
	{
		if (gameServerList_[serverIndex].serverId_ == _serverId)
			return &gameServerList_[serverIndex];
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
GameServerProcessInfo* ServerProcessInfoPackage::GetGameServerProcessInfo(GameServerType_t _gameServerType)
{
	for (int serverIndex = 0; serverIndex < gameServerList_.Size(); ++serverIndex)
	{
		GameServerProcessInfo& gameServerInfo = gameServerList_[serverIndex];

		if (gameServerInfo.gameServerType_ == _gameServerType)
			return &gameServerInfo;
	}

	return nullptr;
}
