/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGMonsterInfo.h>
#include <SteinsGate/Client/SGActionInfo.h>
#include <SteinsGate/Client/SGCharBaseInfo.h>
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
class SGDataManager
{
public:
	void LoadAllConfigs();

	static SGDataManager* get() {
		static SGDataManager* loader;

		if (loader == nullptr) {
			loader = dbg_new SGDataManager;
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
	int getFontCode(const SGString& fontName);
	const SGString& getFontName(int fontCode);
	SGUIElementInfo* getUIElementInfo(int uiElementCode);
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
	SGHashMap<SGString, int> m_FontNameToCodeMap;
	SGHashMap<int, SGString> m_FontCodeToNameMap;
	SGHashMap<int, SGUIElementInfo*> m_UIElementInfoMap;

	SGHashMap<int, SGAnimationInfo> m_CharAnimationInfoMap[CharType::Max];
	SGVector<SGAnimationInfo*> m_CharAnimationList[CharType::Max];
	SGCharBaseInfo* m_CharBaseInfoMap[CharType::Max];
};

