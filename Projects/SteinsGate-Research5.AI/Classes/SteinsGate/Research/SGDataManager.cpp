/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */


#include "SGDataManager.h"

#include <SteinsGate/Research/SGActionInfoLoader.h>
#include <SteinsGate/Research/SGMonsterInfoLoader.h>
#include <SteinsGate/Research/SGCharacterBaseInfoLoader.h>
#include <SteinsGate/Research/SGProjectileInfoLoader.h>
#include <SteinsGate/Research/SGClientInfoLoader.h>
#include <SteinsGate/Research/SGTileInfoLoader.h>
#include <SteinsGate/Research/SGObstacleInfoLoader.h>
#include <SteinsGate/Research/SGMapInfoLoader.h>
#include <SteinsGate/Research/SGAIInfoLoader.h>


void SGDataManager::LoadAllConfigs() {
	// 내가 만든 설정파일 들은 아직 엄청 가벼워서 쓰레드가 필요없다.
	SGProjectileInfoLoader::LoadProjectileInfo(m_ProjectileInfoMap);
	SGActionInfoLoader::LoadActionInfo(m_ActionInfoMap);
	SGMonsterInfoLoader::LoadMonsterInfo(m_MonsterInfoMap);
	SGCharacterBaseInfoLoader::LoadCharacterBaseInfo(m_CharacterBaseInfoMap);
	SGClientInfoLoader::LoadClientInfo(m_ClientInfo);
	SGTileInfoLoader::LoadTileInfo(m_TileInfoMap);
	SGObstacleInfoLoader::LoadObstacleInfo(m_ObstacleInfoMap);
	SGMapInfoLoader::LoadMapInfo(m_MapInfoMap);
	SGAIInfoLoader::LoadAIInfo(m_AIInfoMap);

	m_ActionInfoMap.Values().Extension().ForEach([this](SGActionInfo& info) {
		m_ActionInfoNameMap.Insert(info.ActionName, &info);
	});
}

SGDataManager::SGDataManager() {}

SGDataManager::~SGDataManager() {

	for (int i = 0; i < CharacterType::Max; ++i) {
		DeleteSafe(m_CharacterBaseInfoMap[i]);
	}
}

SGMonsterInfo* SGDataManager::getMonsterInfo(int mobCode) {
	DebugAssertMessage(m_MonsterInfoMap.Exist(mobCode), "해당 몬스터 정보가 존재하지 않습니다.");
	return &m_MonsterInfoMap[mobCode];
}

SGActionInfo* SGDataManager::getActionInfo(int actionCode) {
	DebugAssertMessage(m_ActionInfoMap.Exist(actionCode), "해당 액션 정보(코드)가 존재하지 않습니다.");
	return &m_ActionInfoMap[actionCode];
}

SGActionInfo* SGDataManager::getActionInfo(const SGString& actionName) {
	DebugAssertMessage(m_ActionInfoNameMap.Exist(actionName), "해당 액션 정보(이름)가 존재하지 않습니다.");
	return m_ActionInfoNameMap[actionName];
}

SGProjectileInfo* SGDataManager::getProjectileInfo(int projectileCode) {
	DebugAssertMessage(m_ProjectileInfoMap.Exist(projectileCode), "해당 프로젝틸 정보가 존재하지 않습니다.");
	return &m_ProjectileInfoMap[projectileCode];
}

SGCharacterBaseInfo* SGDataManager::getCharacterBaseInfo(int characterCode) {
	DebugAssertMessage(characterCode >= CharacterType::Begin && characterCode <= CharacterType::End, "해당 캐릭터 타입은 존재하지 않습니다.");
	return m_CharacterBaseInfoMap[characterCode];
}

SGTileInfo* SGDataManager::getTileInfo(int tileCode) {
	DebugAssertMessage(m_TileInfoMap.Exist(tileCode), "해당 타일 정보가 존재하지 않습니다.");
	return &m_TileInfoMap[tileCode];
}

SGObstacleInfo* SGDataManager::getObstacleInfo(int obstacleCode) {
	DebugAssertMessage(m_ObstacleInfoMap.Exist(obstacleCode), "해당 옵스터클 정보가 존재하지 않습니다.");
	return &m_ObstacleInfoMap[obstacleCode];
}

SGMapInfo* SGDataManager::getMapInfo(int mapCode) {
	DebugAssertMessage(m_TileInfoMap.Exist(mapCode), "해당 맵 정보가 존재하지 않습니다.");
	return &m_MapInfoMap[mapCode];
}

SGAIInfo* SGDataManager::getAIInfo(int aiCode) {
	DebugAssertMessage(m_AIInfoMap.Exist(aiCode), "해당 AI 정보(코드)가 존재하지 않습니다.");
	return &m_AIInfoMap[aiCode];
}