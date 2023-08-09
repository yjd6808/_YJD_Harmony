/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <SteinsGate/Client/ProjectileListener.h>
#include <SteinsGate/Client/CharacterListener.h>

class ActorListenerManager final : public JCore::SingletonPointer<ActorListenerManager>
{
private:
	friend class TSingleton;
	ActorListenerManager();
	~ActorListenerManager() override;
public:
	void init();

	ProjectileListener* createProjectileListener(int projectileListenerCode);
	CharacterListener* createCharacterListener(CharType_t charType);				
private:
	SGHashMap<int, ProjectileListener*> m_hProjectileListenerMap;
	SGHashMap<int, CharacterListener*> m_hCharacterListenerMap;
};


