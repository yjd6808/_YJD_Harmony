/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:58:45 AM
 * =====================
 *
 */

#include "Core.h"
#include "ServerProcessInfo.h"

ServerProcessInfo* ServerProcessInfoPackage::getServerProcessInfo(int serverId) {
	if (Auth.ServerId == serverId) { return &Auth; }
	if (Lobby.ServerId == serverId) { return &Lobby; }
	if (Center.ServerId == serverId) { return &Center; }
	for (int i = 0; i < GameServerList.Size(); ++i) {
		if (GameServerList[i].ServerId == serverId) {
			return &GameServerList[i];
		}
	}
	return nullptr;
}

GameServerProcessInfo* ServerProcessInfoPackage::getGameServerProcessInfo(GameServerType_t gameServerType) {

	for (int i = 0; i < GameServerList.Size(); ++i) {
		GameServerProcessInfo& info = GameServerList[i];

		if (info.GameServerType == gameServerType) {
			return &info;
		}
	}
	return nullptr;
}
