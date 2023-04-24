/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/ProjectileListener.h>

class ActorListenerManager
{
private:
	ActorListenerManager();
public:
	~ActorListenerManager();

	void init();

	static ActorListenerManager* get() {
		static ActorListenerManager* s_pManager;

		if (s_pManager == nullptr) {
			s_pManager = dbg_new ActorListenerManager;
		}
		return s_pManager;
	}

	ProjectileListener* createProjectileListener(int projectileListenerCode);

private:
	SGHashMap<int, ProjectileListener*> m_hProjectileListenerMap;
};


