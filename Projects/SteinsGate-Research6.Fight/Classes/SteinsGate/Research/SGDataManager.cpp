/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGDataManager.h"

#include <SteinsGate/Research/SGActionInfoLoader.h>
#include <SteinsGate/Research/SGMonsterInfoLoader.h>
#include <SteinsGate/Research/SGCharBaseInfoLoader.h>
#include <SteinsGate/Research/SGProjectileInfoLoader.h>
#include <SteinsGate/Research/SGClientInfoLoader.h>
#include <SteinsGate/Research/SGTileInfoLoader.h>
#include <SteinsGate/Research/SGObstacleInfoLoader.h>
#include <SteinsGate/Research/SGMapInfoLoader.h>
#include <SteinsGate/Research/SGAIInfoLoader.h>
#include <SteinsGate/Research/SGAttackDataInfoLoader.h>
#include <SteinsGate/Research/SGCharAnimationInfoLoader.h>
#include <SteinsGate/Research/SGEffectInfoLoader.h>

SGDataManager::SGDataManager() {}
SGDataManager::~SGDataManager() {

	for (int i = 0; i < CharType::Max; ++i) {
		DeleteSafe(m_CharBaseInfoMap[i]);
	}
}

void SGDataManager::LoadAllConfigs() {
	// 내가 만든 설정파일 들은 아직 엄청 가벼워서 쓰레드가 필요없다.


	if (SGAttackDataInfoLoader::LoadAttackDataInfo(m_AttackDataInfoMap) &&
		SGEffectInfoLoader::LoadEffectInfo(m_EffectInfoMap) &&
		SGProjectileInfoLoader::LoadProjectileInfo(m_ProjectileInfoMap) &&		// 프로젝틸은 이펙트, 어택 데이터를 참조하므로 이후에 로딩
		SGActionInfoLoader::LoadActionInfo(m_ActionInfoMap) &&
		SGMonsterInfoLoader::LoadMonsterInfo(m_MonsterInfoMap) &&
		SGCharBaseInfoLoader::LoadCharBaseInfo(m_CharBaseInfoMap) &&
		SGCharAnimationInfoLoader::LoadCharAnimationInfo(m_CharAnimationInfoMap, m_CharAnimationList) &&
		SGClientInfoLoader::LoadClientInfo(m_ClientInfo) && 
		SGTileInfoLoader::LoadTileInfo(m_TileInfoMap) && 
		SGObstacleInfoLoader::LoadObstacleInfo(m_ObstacleInfoMap) &&
		SGMapInfoLoader::LoadMapInfo(m_MapInfoMap) &&
		SGAIInfoLoader::LoadAIInfo(m_AIInfoMap)) {
		Log("==== 모든 기획파일 로딩완료 ====\n");
	} else {
		DebugAssertMsg(false, "기획파일 로딩 실패");
	}
}



SGMonsterInfo* SGDataManager::getMonsterInfo(int mobCode) {
	DebugAssertMsg(m_MonsterInfoMap.Exist(mobCode), "해당 몬스터 정보가 존재하지 않습니다.");
	return &m_MonsterInfoMap[mobCode];
}

SGActionInfo* SGDataManager::getActionInfo(int actionCode) {
	DebugAssertMsg(m_ActionInfoMap.Exist(actionCode), "해당 액션 정보(코드)가 존재하지 않습니다.");
	return &m_ActionInfoMap[actionCode];
}


SGProjectileInfo* SGDataManager::getProjectileInfo(int projectileCode) {
	DebugAssertMsg(m_ProjectileInfoMap.Exist(projectileCode), "해당 프로젝틸 정보가 존재하지 않습니다.");
	return &m_ProjectileInfoMap[projectileCode];
}

SGCharBaseInfo* SGDataManager::getCharBaseInfo(int charCode) {
	DebugAssertMsg(charCode >= CharType::Begin && charCode <= CharType::End, "해당 캐릭터 타입은 존재하지 않습니다.");
	return m_CharBaseInfoMap[charCode];
}

SGAnimationInfo* SGDataManager::getCharAnimationInfo(int charCode, int charAnimationCode) {
	DebugAssertMsg(charCode >= CharType::Begin && charCode <= CharType::End, "해당 캐릭터 타입은 존재하지 않습니다.");
	DebugAssertMsg(m_CharAnimationInfoMap[charCode].Exist(charAnimationCode), "해당 캐릭터 애니메이션 정보가 존재하지 않습니다.");
	return &m_CharAnimationInfoMap[charCode][charAnimationCode];
}

SGVector<SGAnimationInfo*>& SGDataManager::getCharAnimationInfoList(int charCode) {
	DebugAssertMsg(charCode >= CharType::Begin && charCode <= CharType::End, "해당 캐릭터 타입은 존재하지 않습니다.");
	return m_CharAnimationList[charCode];
}


SGTileInfo* SGDataManager::getTileInfo(int tileCode) {
	DebugAssertMsg(m_TileInfoMap.Exist(tileCode), "해당 타일 정보가 존재하지 않습니다.");
	return &m_TileInfoMap[tileCode];
}

SGObstacleInfo* SGDataManager::getObstacleInfo(int obstacleCode) {
	DebugAssertMsg(m_ObstacleInfoMap.Exist(obstacleCode), "해당 옵스터클 정보가 존재하지 않습니다.");
	return &m_ObstacleInfoMap[obstacleCode];
}

SGMapInfo* SGDataManager::getMapInfo(int mapCode) {
	DebugAssertMsg(m_TileInfoMap.Exist(mapCode), "해당 맵 정보가 존재하지 않습니다.");
	return &m_MapInfoMap[mapCode];
}

SGAIInfo* SGDataManager::getAIInfo(int aiCode) {
	DebugAssertMsg(m_AIInfoMap.Exist(aiCode), "해당 AI 정보(코드)가 존재하지 않습니다.");
	return &m_AIInfoMap[aiCode];
}

SGAttackDataInfo* SGDataManager::getAttackDataInfo(int attackDataCode) {
	DebugAssertMsg(m_AttackDataInfoMap.Exist(attackDataCode), "해당 공격 데이터 정보가 존재하지 않습니다.");
	return &m_AttackDataInfoMap[attackDataCode];
}

SGEffectInfo* SGDataManager::getEffectInfo(int effectCode) {
	DebugAssertMsg(m_EffectInfoMap.Exist(effectCode), "해당 이펙트 코드에 맞는 이펙트가 없다@~@~!@~!@~");
	return &m_EffectInfoMap[effectCode];
}
