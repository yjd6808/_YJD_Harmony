/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGProjectileListener.h>

class SGActorListenerManager
{
private:
	SGActorListenerManager();
public:
	~SGActorListenerManager();

	void init();

	static SGActorListenerManager* get() {
		static SGActorListenerManager* s_pManager;

		if (s_pManager == nullptr) {
			s_pManager = dbg_new SGActorListenerManager;
		}
		return s_pManager;
	}

	SGProjectileListener* createProjectileListener(int projectileListenerCode);

private:
	SGHashMap<int, SGProjectileListener*> m_hProjectileListenerMap;
};


