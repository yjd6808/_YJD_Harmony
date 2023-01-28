/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGMonsterInfo.h>
#include <SteinsGate/Research/SGActionInfo.h>
#include <SteinsGate/Research/SGCharacterBaseInfo.h>
#include <SteinsGate/Research/SGProjectileInfo.h>
#include <SteinsGate/Research/SGClientInfo.h>
#include <SteinsGate/Research/SGTileInfo.h>
#include <SteinsGate/Research/SGObstacleInfo.h>
#include <SteinsGate/Research/SGMapInfo.h>


class SGDataManager
{
public:
	void LoadAllConfigs();

	static SGDataManager* getInstance() {
		static SGDataManager* loader;

		if (loader == nullptr) {
			loader = new SGDataManager;
			loader->LoadAllConfigs();
		}
		return loader;
	}

	SGDataManager();
	~SGDataManager();
	

	SGMonsterInfo* getMonsterInfo(int mobCode);
	SGActionInfo* getActionInfo(int actionCode);
	SGActionInfo* getActionInfo(const SGString& actionName);
	SGProjectileInfo* getProjectileInfo(int projectileCode);
	SGCharacterBaseInfo* getCharacterBaseInfo(int characterCode);
	SGClientInfo* getClientInfo() { return &m_ClientInfo; }
	SGTileInfo* getTileInfo(int tileCode);
	SGObstacleInfo* getObstacleInfo(int obstacleCode);
	SGMapInfo* getMapInfo(int mapCode);


private:
	SGClientInfo m_ClientInfo;
	SGHashMap<int, SGMonsterInfo> m_MonsterInfoMap;
	SGHashMap<int, SGActionInfo> m_ActionInfoMap;
	SGHashMap<int, SGProjectileInfo> m_ProjectileInfoMap;
	SGHashMap<int, SGTileInfo> m_TileInfoMap;
	SGHashMap<int, SGObstacleInfo> m_ObstacleInfoMap;
	SGHashMap<int, SGMapInfo> m_MapInfoMap;

	SGHashMap<SGString, SGActionInfo*> m_ActionInfoNameMap;

	SGCharacterBaseInfo* m_CharacterBaseInfoMap[CharacterType::Max];
};


