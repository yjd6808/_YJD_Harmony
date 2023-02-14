/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:01:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGDataManager.h"

#include <SteinsGate/Client/SGActionInfoLoader.h>
#include <SteinsGate/Client/SGMonsterInfoLoader.h>
#include <SteinsGate/Client/SGCharBaseInfoLoader.h>
#include <SteinsGate/Client/SGProjectileInfoLoader.h>
#include <SteinsGate/Client/SGClientInfoLoader.h>
#include <SteinsGate/Client/SGTileInfoLoader.h>
#include <SteinsGate/Client/SGObstacleInfoLoader.h>
#include <SteinsGate/Client/SGMapInfoLoader.h>
#include <SteinsGate/Client/SGAIInfoLoader.h>
#include <SteinsGate/Client/SGAttackDataInfoLoader.h>
#include <SteinsGate/Client/SGCharAnimationInfoLoader.h>
#include <SteinsGate/Client/SGEffectInfoLoader.h>
#include <SteinsGate/Client/SGFontLoader.h>
#include <SteinsGate/Client/SGUIInfoLoader.h>

SGDataManager::SGDataManager() {}
SGDataManager::~SGDataManager() {

	for (int i = 0; i < CharType::Max; ++i) {
		DeleteSafe(m_CharBaseInfoMap[i]);
	}

	m_UIElementInfoMap.Values().Extension().ForEach([](SGUIElementInfo* info) { delete info; });
}

void SGDataManager::LoadAllConfigs() {
	// 내가 만든 설정파일 들은 아직 엄청 가벼워서 쓰레드가 필요없다.


	if (SGFontLoader::LoadFontInfo(m_FontNameToCodeMap, m_FontCodeToNameMap) &&
		SGAttackDataInfoLoader::LoadAttackDataInfo(m_AttackDataInfoMap) &&
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
		SGAIInfoLoader::LoadAIInfo(m_AIInfoMap) &&
		SGUIInfoLoader::LoadUIInfo(m_UIElementInfoMap, m_UIGroupInfoMap)) {
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

int SGDataManager::getFontCode(const SGString& fontName) {
	DebugAssertMsg(m_FontNameToCodeMap.Exist(fontName), "해당 폰트이름에 맞는 폰트 코드가 없습니다.");
	return m_FontNameToCodeMap[fontName];
}

const SGString& SGDataManager::getFontName(int fontCode) {
	DebugAssertMsg(m_FontCodeToNameMap.Exist(fontCode), "해당 폰트코드에 맞는 폰트 이름이 없습니다.");
	return m_FontCodeToNameMap[fontCode];
}

SGUIElementInfo* SGDataManager::getUIElementInfo(int uiElementCode) {
	DebugAssertMsg(m_UIElementInfoMap.Exist(uiElementCode), "해당 UI 엘리먼트 코드에 맞는 정보가 없습니다.");
	return m_UIElementInfoMap[uiElementCode];
}

SGUIGroupInfo* SGDataManager::getUIGroupInfo(int uiGroupCode) {
	DebugAssertMsg(m_UIGroupInfoMap.Exist(uiGroupCode), "해당 UI 그룹 코드에 맞는 정보가 없습니다.");
	return &m_UIGroupInfoMap[uiGroupCode];
}

