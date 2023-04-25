/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/ProjectileListener.h>

class ActorListenerManager final : public JCore::SingletonPointer<ActorListenerManager>
{
private:
	friend class TSingleton;
	ActorListenerManager();
	~ActorListenerManager() override;
public:
	void init();
	ProjectileListener* createProjectileListener(int projectileListenerCode);
private:
	SGHashMap<int, ProjectileListener*> m_hProjectileListenerMap;
};


