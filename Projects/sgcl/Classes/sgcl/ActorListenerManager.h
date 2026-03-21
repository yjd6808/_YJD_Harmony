/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include "sgcl/CharacterListener.h"
#include "sgcl/MonsterListener.h"
#include "sgcl/ProjectileListener.h"

class ActorListenerManager final : public jc::SingletonPointer<ActorListenerManager>
{
private:
	using CharacterFactoryMap = jc::HashMap<int, CharacterListener::IFactory*>;
	using MonsterFactoryMap = jc::HashMap<int, MonsterListener::IFactory*>;
	using ProjectileFactoryMap = jc::HashMap<int, ProjectileListener::IFactory*>;

	friend class TSingleton;
	ActorListenerManager();
	~ActorListenerManager();

public:
	void Init();

	CharacterListener*		CreateCharacterListener(Character* _pCharacter);
	MonsterListener*		CreateMonsterListener(Monster* _pMonster);
	ProjectileListener*		CreateProjectileListener(Projectile* _pProjectile, Actor* _pSpawner = nullptr);

private:
	ProjectileFactoryMap	projectileListenerMap_;
	MonsterFactoryMap		monsterListenerMap_;
	CharacterFactoryMap		characterListenerMap_;
};
