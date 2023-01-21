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

struct SGConfigManager
{
public:
	void LoadAllConfigs();

	static SGConfigManager* getInstance() {
		static SGConfigManager* loader;

		if (loader == nullptr) {
			loader = new SGConfigManager;
			loader->LoadAllConfigs();
		}
		return loader;
	}

	SGMonsterInfo* getMonsterInfo(int mobCode);
	SGActionInfo* getActionInfo(int actionCode);
	SGActionInfo* getActionInfo(const SGString& actionName);
	SGProjectileInfo* getProjectileInfo(int projectileCode);
	SGCharacterBaseInfo* getCharacterBaseInfo(int characterCode);

private:
	SGHashMap<int, SGMonsterInfo> m_MonsterInfoMap;
	SGHashMap<int, SGActionInfo> m_ActionInfoMap;
	SGHashMap<int, SGProjectileInfo> m_ProjectInfoMap;

	SGHashMap<SGString, SGActionInfo*> m_ActionInfoNameMap;

	SGCharacterBaseInfo m_CharacterBaseInfoMap[CharacterType::Max];
};

