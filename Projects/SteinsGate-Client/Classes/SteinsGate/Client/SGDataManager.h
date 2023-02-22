/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/DataManagerAbstract.h>

#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Client/SGMobInfo.h>
#include <SteinsGate/Client/SGActionInfo.h>
#include <SteinsGate/Client/SGCharInfo.h>
#include <SteinsGate/Client/SGProjectileInfo.h>
#include <SteinsGate/Client/SGClientInfo.h>
#include <SteinsGate/Client/SGTileInfo.h>
#include <SteinsGate/Client/SGObstacleInfo.h>
#include <SteinsGate/Client/SGMapInfo.h>
#include <SteinsGate/Client/SGAIInfo.h>
#include <SteinsGate/Client/SGAttackDataInfo.h>
#include <SteinsGate/Client/SGEffectInfo.h>
#include <SteinsGate/Client/SGUIInfo.h>

using AnimationList = SGVector<SGAnimationInfo*>;

class SGDataManager : public DataManagerAbstract
{
public:
	static SGDataManager* get() {
		static SGDataManager* loader;

		if (loader == nullptr) {
			loader = dbg_new SGDataManager;
			loader->initializeLoader();
		}
		return loader;
	}

	SGDataManager();
	~SGDataManager() override = default;

	void initializeLoader() override;

	SGMobInfo* getMonsterInfo(int mobCode);
	SGActionInfo* getActionInfo(int actionCode);
	SGProjectileInfo* getProjectileInfo(int projectileCode);
	SGCharInfo* getCharInfo(int charCode);
	SGAnimationInfo* getCharAnimationInfo(int charAnimationCode);
	SGVector<SGAnimationInfo*>& getCharAnimationInfoList(int charCode);
	SGClientInfo* getClientInfo(int code);
	SGTileInfo* getTileInfo(int tileCode);
	SGObstacleInfo* getObstacleInfo(int obstacleCode);
	SGMapInfo* getMapInfo(int mapCode);
	SGAIInfo* getAIInfo(int aiCode);
	SGAttackDataInfo* getAttackDataInfo(int attackDataCode);
	SGEffectInfo* getEffectInfo(int effectCode);
	SGUIElementInfo* getUIElementInfo(int uiElementCode);


};

