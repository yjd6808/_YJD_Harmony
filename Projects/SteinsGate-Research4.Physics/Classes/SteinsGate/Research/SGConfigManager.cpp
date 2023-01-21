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

SGMonsterInfo* SGConfigManager::getMonsterInfo(int mobCode) {
	DebugAssertMessage(m_MonsterInfoMap.Exist(mobCode), "해당 몬스터 정보가 존재하지 않습니다.");
	return &m_MonsterInfoMap[mobCode];
}

SGActionInfo* SGConfigManager::getActionInfo(int actionCode) {
	DebugAssertMessage(m_ActionInfoMap.Exist(actionCode), "해당 액션 정보가 존재하지 않습니다.");
	return &m_ActionInfoMap[actionCode];
}

SGProjectileInfo* SGConfigManager::getProjectileInfo(int projectileCode) {
	DebugAssertMessage(m_ProjectInfoMap.Exist(projectileCode), "해당 프로젝틸 정보가 존재하지 않습니다.");
	return &m_ProjectInfoMap[projectileCode];
}

SGCharacterBaseInfo* SGConfigManager::getCharacterBaseInfo(int characterCode) {
	DebugAssertMessage(characterCode >= CharacterType::Begin && characterCode <= CharacterType::End, "해당 캐릭터 타입은 존재하지 않습니다.");
	return &m_CharacterBaseInfoMap[characterCode];
}

