/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/DataManagerAbstract.h>

#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Client/MobInfo.h>
#include <SteinsGate/Client/ActionInfo.h>
#include <SteinsGate/Client/CharInfo.h>
#include <SteinsGate/Client/ProjectileInfo.h>
#include <SteinsGate/Client/ClientInfo.h>
#include <SteinsGate/Client/TileInfo.h>
#include <SteinsGate/Client/ObstacleInfo.h>
#include <SteinsGate/Client/MapInfo.h>
#include <SteinsGate/Client/AIInfo.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/EffectInfo.h>
#include <SteinsGate/Client/UIInfo.h>

using AnimationList = SGVector<AnimationInfo*>;

class DataManager : public DataManagerAbstract
{
public:
	static DataManager* get() {
		static DataManager* loader;

		if (loader == nullptr) {
			loader = dbg_new DataManager;
			loader->initializeLoader();
		}
		return loader;
	}

	DataManager();
	~DataManager() override = default;

	void initializeLoader() override;

	MobInfo* getMonsterInfo(int mobCode);
	ActionInfo* getActionInfo(int actionCode);
	ProjectileInfo* getProjectileInfo(int projectileCode);
	CharInfo* getCharInfo(int charCode);
	AnimationInfo* getCharAnimationInfo(int charAnimationCode);
	SGVector<AnimationInfo*>& getCharAnimationInfoList(int charCode);
	ClientInfo* getClientInfo(int code);
	TileInfo* getTileInfo(int tileCode);
	ObstacleInfo* getObstacleInfo(int obstacleCode);
	MapInfo* getMapInfo(int mapCode);
	AIInfo* getAIInfo(int aiCode);
	AttackDataInfo* getAttackDataInfo(int attackDataCode);
	EffectInfo* getEffectInfo(int effectCode);
	UIElementInfo* getUIElementInfo(int uiElementCode);


};

