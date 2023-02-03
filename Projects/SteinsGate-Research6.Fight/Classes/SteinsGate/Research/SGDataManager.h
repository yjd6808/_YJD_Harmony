/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGMonsterInfo.h>
#include <SteinsGate/Research/SGActionInfo.h>
#include <SteinsGate/Research/SGCharBaseInfo.h>
#include <SteinsGate/Research/SGProjectileInfo.h>
#include <SteinsGate/Research/SGClientInfo.h>
#include <SteinsGate/Research/SGTileInfo.h>
#include <SteinsGate/Research/SGObstacleInfo.h>
#include <SteinsGate/Research/SGMapInfo.h>
#include <SteinsGate/Research/SGAIInfo.h>
#include <SteinsGate/Research/SGAttackDataInfo.h>
#include <SteinsGate/Research/SGEffectInfo.h>

using AnimationList = SGVector<SGAnimationInfo*>;
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
	SGProjectileInfo* getProjectileInfo(int projectileCode);
	SGCharBaseInfo* getCharBaseInfo(int charCode);
	SGAnimationInfo* getCharAnimationInfo(int charCode, int charAnimationCode);
	SGVector<SGAnimationInfo*>& getCharAnimationInfoList(int charCode);
	SGClientInfo* getClientInfo() { return &m_ClientInfo; }
	SGTileInfo* getTileInfo(int tileCode);
	SGObstacleInfo* getObstacleInfo(int obstacleCode);
	SGMapInfo* getMapInfo(int mapCode);
	SGAIInfo* getAIInfo(int aiCode);
	SGAttackDataInfo* getAttackDataInfo(int attackDataCode);
	SGEffectInfo* getEffectInfo(int effectCode);
private:
	SGClientInfo m_ClientInfo;
	SGHashMap<int, SGMonsterInfo> m_MonsterInfoMap;
	SGHashMap<int, SGActionInfo> m_ActionInfoMap;
	SGHashMap<int, SGProjectileInfo> m_ProjectileInfoMap;
	SGHashMap<int, SGTileInfo> m_TileInfoMap;
	SGHashMap<int, SGObstacleInfo> m_ObstacleInfoMap;
	SGHashMap<int, SGMapInfo> m_MapInfoMap;
	SGHashMap<int, SGAIInfo> m_AIInfoMap;
	SGHashMap<int, SGAttackDataInfo> m_AttackDataInfoMap;
	SGHashMap<int, SGEffectInfo> m_EffectInfoMap;

	SGHashMap<int, SGAnimationInfo> m_CharAnimationInfoMap[CharType::Max];
	SGVector<SGAnimationInfo*> m_CharAnimationList[CharType::Max];
	SGCharBaseInfo* m_CharBaseInfoMap[CharType::Max];
};

