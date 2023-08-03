/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:58:45 AM
 * =====================
 *
 */

#include "Core.h"
#include "ServerProcessInfo.h"

GameServerProcessInfo* ServerProcessInfoPackage::getGameServerProcessInfo(GameServerType_t gameServerType) {

	for (int i = 0; i < GameList.Size(); ++i) {
		GameServerProcessInfo& info = GameList[i];

		if (info.Type == gameServerType) {
			return &info;
		}
	}
	return nullptr;
}
