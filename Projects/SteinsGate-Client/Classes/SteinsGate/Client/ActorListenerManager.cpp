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

//////////////////////////////////////////////////////////////////////////////////////////
ActorListenerManager::ActorListenerManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorListenerManager::~ActorListenerManager()
{
	characterListenerMap_.ForEachValueDelete();
	monsterListenerMap_.ForEachValueDelete();
	projectileListenerMap_.ForEachValueDelete();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerManager::Init()
{
	// 캐릭터 리스너 등록
	characterListenerMap_.Insert(DEF_CHARACTER_LISTENER_GUNNER, dbg_new CharacterListener_Gunner::Factory);

	// 몬스터 리스너 등록
	monsterListenerMap_.Insert(DEF_MONSTER_LISTENER_NORMAL_GOBLIN, dbg_new MonsterListener_NormalGoblin::Factory);
	monsterListenerMap_.Insert(DEF_MONSTER_LISTENER_DARK_GOBLIN, dbg_new MonsterListener_DarkGoblin::Factory);

	// 프로젝틸 리스너 등록
	projectileListenerMap_.Insert(DEF_PROJECTILE_LISTENER_GUNNER_BULLET, dbg_new ProjectileListener_GunnerBullet::Factory);
}

//////////////////////////////////////////////////////////////////////////////////////////
CharacterListener* ActorListenerManager::CreateCharacterListener(Character* _pCharacter)
{
	const int characterCode = _pCharacter->GetCode();
	CharacterListener::IFactory** pFactory = characterListenerMap_.Find(characterCode);

	if (pFactory == nullptr)
	{
		DebugAssertMsg(false, "해당 %sListener를 찾지 못했습니다.", ActorType::Name[ActorType::Character]);
		return nullptr;
	}

	return (*pFactory)->Create(_pCharacter);
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterListener* ActorListenerManager::CreateMonsterListener(Monster* _pMonster)
{
	const int monsterCode = _pMonster->GetCode();
	MonsterListener::IFactory** pFactory = monsterListenerMap_.Find(monsterCode);

	if (pFactory == nullptr)
	{
		DebugAssertMsg(false, "해당 %sListener를 찾지 못했습니다.", ActorType::Name[ActorType::Monster]);
		return nullptr;
	}

	return (*pFactory)->Create(_pMonster);
}

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileListener* ActorListenerManager::CreateProjectileListener(Projectile* _pProjectile, Actor* _pSpawner /* = nullptr */)
{
	const int projectileListenerCode = _pProjectile->GetListenerCode();
	ProjectileListener::IFactory** pFactory = projectileListenerMap_.Find(projectileListenerCode);

	if (pFactory == nullptr)
	{
		DebugAssertMsg(false, "해당 %sListener를 찾지 못했습니다.", ActorType::Name[ActorType::Projectile]);
		return nullptr;
	}

	return (*pFactory)->Create(_pProjectile, _pSpawner);
}
