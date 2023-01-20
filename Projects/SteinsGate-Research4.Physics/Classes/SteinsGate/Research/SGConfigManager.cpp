/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */


#include "SGConfigManager.h"

#include <SteinsGate/Research/SGActionInfoLoader.h>
#include <SteinsGate/Research/SGMonsterInfoLoader.h>
#include <SteinsGate/Research/SGCharacterBaseInfoLoader.h>
#include <SteinsGate/Research/SGProjectileInfoLoader.h>

void SGConfigManager::LoadAllConfigs() {
	// 내가 만든 설정파일 들은 아직 엄청 가벼워서 쓰레드가 필요없다.
	SGProjectileInfoLoader::LoadProjectileInfo(m_ProjectInfoMap);
	SGActionInfoLoader::LoadActionInfo(m_ActionInfoMap);
	SGMonsterInfoLoader::LoadMonsterInfo(m_MonsterInfoMap);
	SGCharacterBaseInfoLoader::LoadCharacterBaseInfo(m_CharacterBaseInfoMap);
}

SGMonsterInfo* SGConfigManager::getMonsterInfo(const int mobCode) {
	return nullptr;
}

SGActionInfo* SGConfigManager::getActionInfo(const int actionCode) {
	return nullptr;
}

SGProjectileInfo* SGConfigManager::getProjectileInfo(const int projectileCode) {
	return nullptr;
}

