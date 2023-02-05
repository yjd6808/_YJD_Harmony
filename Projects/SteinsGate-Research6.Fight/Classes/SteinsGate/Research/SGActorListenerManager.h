/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:38:29 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Research/SGProjectileListener.h>

class SGActorListenerManager
{
private:
	SGActorListenerManager();
public:
	~SGActorListenerManager();

	void initialize();

	static SGActorListenerManager* get() {
		static SGActorListenerManager* s_pManager;

		if (s_pManager == nullptr) {
			s_pManager = new SGActorListenerManager;
			s_pManager->initialize();
		}
		return s_pManager;
	}

	SGProjectileListener* createProjectileListener(int projectileListenerCode);

private:
	SGHashMap<int, SGProjectileListener*> m_hProjectileListenerMap;
};


