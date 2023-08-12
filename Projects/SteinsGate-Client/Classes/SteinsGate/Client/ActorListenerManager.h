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
	~ActorListenerManager() override;
public:
	void init();

	CharacterListener* createCharacterListener(Character* character);
	MonsterListener* createMonsterListener(Monster* monster);
	ProjectileListener* createProjectileListener(Projectile* projectile, Actor* spawner = nullptr);
private:
	ProjectileFactoryMap m_hProjectileListenerMap;
	MonsterFactoryMap m_hMonsterListenerMap;
	CharacterFactoryMap m_hCharacterListenerMap;
};


