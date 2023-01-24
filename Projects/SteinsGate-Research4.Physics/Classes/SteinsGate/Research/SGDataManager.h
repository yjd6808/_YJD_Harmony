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

struct SGDataManager
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

	SGMonsterInfo* getMonsterInfo(int mobCode);
	SGActionInfo* getActionInfo(int actionCode);
	SGActionInfo* getActionInfo(const SGString& actionName);
	SGProjectileInfo* getProjectileInfo(int projectileCode);
	SGCharacterBaseInfo* getCharacterBaseInfo(int characterCode);
	SGClientInfo* getClientInfo() { return &m_ClientInfo; }

private:
	SGClientInfo m_ClientInfo;
	SGHashMap<int, SGMonsterInfo> m_MonsterInfoMap;
	SGHashMap<int, SGActionInfo> m_ActionInfoMap;
	SGHashMap<int, SGProjectileInfo> m_ProjectInfoMap;

	SGHashMap<SGString, SGActionInfo*> m_ActionInfoNameMap;

	SGCharacterBaseInfo m_CharacterBaseInfoMap[CharacterType::Max];
};

