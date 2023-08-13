/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:44:55 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "ActorListenerManager.h"

#include <SteinsGate/Client/Define_ActorListener.h>

// 캐릭터 리스너
#include <SteinsGate/Client/ProjectileListener_GunnerBullet.h>

// 몬스터 리스너
#include <SteinsGate/Client/MonsterListener_NormalGoblin.h>
#include <SteinsGate/Client/MonsterListener_DarkGoblin.h>

// 프로젝틸 리스너
#include <SteinsGate/Client/CharacterListener_Gunner.h>

ActorListenerManager::ActorListenerManager() {}

ActorListenerManager::~ActorListenerManager() {
	m_hCharacterListenerMap.ForEachValueDelete();
	m_hMonsterListenerMap.ForEachValueDelete();
	m_hProjectileListenerMap.ForEachValueDelete();
}

void ActorListenerManager::init() {
	// 캐릭터 리스너 등록
	m_hCharacterListenerMap.Insert(DEF_CHARACTER_LISTENER_GUNNER, dbg_new CharacterListener_Gunner::Factory);

	// 몬스터 리스너 등록
	m_hMonsterListenerMap.Insert(DEF_MONSTER_LISTENER_NORMAL_GOBLIN, dbg_new MonsterListener_NormalGoblin::Factory);
	m_hMonsterListenerMap.Insert(DEF_MONSTER_LISTENER_DARK_GOBLIN, dbg_new MonsterListener_DarkGoblin::Factory);

	// 프로젝틸 리스너 등록
	m_hProjectileListenerMap.Insert(DEF_PROJECTILE_LISTENER_GUNNER_BULLET, dbg_new ProjectileListener_GunnerBullet::Factory);
}

CharacterListener* ActorListenerManager::createCharacterListener(Character* character) {
	const int iCharacterCode = character->getCode();
	CharacterListener::IFactory** ppFactory = m_hCharacterListenerMap.Find(iCharacterCode);

	if (ppFactory == nullptr) {
		DebugAssertMsg(false, "해당 %sListener를 찾지 못했습니다.", ActorType::Name[ActorType::Character]);
		return nullptr;
	}

	return (*ppFactory)->create(character);
}

MonsterListener* ActorListenerManager::createMonsterListener(Monster* monster) {
	const int iMonsterCode = monster->getCode();
	MonsterListener::IFactory** ppFactory = m_hMonsterListenerMap.Find(iMonsterCode);

	if (ppFactory == nullptr) {
		DebugAssertMsg(false, "해당 %sListener를 찾지 못했습니다.", ActorType::Name[ActorType::Monster]);
		return nullptr;
	}

	return (*ppFactory)->create(monster);
}

ProjectileListener* ActorListenerManager::createProjectileListener(Projectile* projectile, Actor* spawner /* = nullptr */) {
	const int iProjectileListenerCode = projectile->getListenerCode();
	ProjectileListener::IFactory** ppFactory = m_hProjectileListenerMap.Find(iProjectileListenerCode);

	if (ppFactory == nullptr) {
		DebugAssertMsg(false, "해당 %sListener를 찾지 못했습니다.", ActorType::Name[ActorType::Projectile]);
		return nullptr;
	}

	return (*ppFactory)->create(projectile, spawner);
}


