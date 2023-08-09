/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:44:55 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "ActorListenerManager.h"

#include <SteinsGate/Client/ActorListenerDefine.h>

// 캐릭터 리스너
#include <SteinsGate/Client/ProjectileListener_GunnerBullet.h>

// 프로젝틸 리스너
#include <SteinsGate/Client/CharacterListener_Gunner.h>

ActorListenerManager::ActorListenerManager() {}

ActorListenerManager::~ActorListenerManager() {
	m_hProjectileListenerMap.ForEachValueDelete();
	m_hCharacterListenerMap.ForEachValueDelete();
}

void ActorListenerManager::init() {
	// 캐릭터 리스너 등록
	m_hCharacterListenerMap.Insert(CharType::Gunner, dbg_new CharacterListener_Gunner);

	// 몬스터 리스너 등록

	// 프로젝틸 리스너 등록
	m_hProjectileListenerMap.Insert(PROJECTILE_LISTENER_GUNNER_BULLET, dbg_new ProjectileListener_GunnerBullet);

	
}

ProjectileListener* ActorListenerManager::createProjectileListener(int projectileListenerCode) {
	DebugAssertMsg(m_hProjectileListenerMap.Exist(projectileListenerCode), "해당 코드의 프로젝틸 리스너가 존재하지 않습니다.");
	ProjectileListener* pListener =  dynamic_cast<ProjectileListener*>(m_hProjectileListenerMap[projectileListenerCode]->createNew());
	DebugAssertMsg(pListener, "오잉? 프로젝틸 리스너가 아닌 타입이 프로젝틸 리스너 목록에 존재합니다. 수정이 필요합니다");
	return pListener;
}

CharacterListener* ActorListenerManager::createCharacterListener(CharType_t charType) {
	DebugAssertMsg(m_hCharacterListenerMap.Exist(charType), "해당 코드의 %s 리스너가 존재하지 않습니다.", CharType::Name[charType]);
	CharacterListener* pListener = dynamic_cast<CharacterListener*>(m_hCharacterListenerMap[charType]->createNew());
	DebugAssertMsg(pListener, "오잉? 프로젝틸 리스너가 아닌 타입이 프로젝틸 리스너 목록에 존재합니다. 수정이 필요합니다");
	return pListener;
}
