/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/CharacterListener.h>
#include <SteinsGate/Client/MonsterListener.h>
#include <SteinsGate/Client/ProjectileListener.h>

class ActorListenerManager final : public JCore::SingletonPointer<ActorListenerManager>
{
private:
	using CharacterFactoryMap = SGHashMap<int, CharacterListener::IFactory*>;
	using MonsterFactoryMap = SGHashMap<int, MonsterListener::IFactory*>;
	using ProjectileFactoryMap = SGHashMap<int, ProjectileListener::IFactory*>;

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
