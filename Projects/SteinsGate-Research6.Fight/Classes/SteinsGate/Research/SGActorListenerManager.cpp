/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:44:55 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGActorListenerManager.h"

#include <SteinsGate/Research/SGActorListenerDefine.h>
#include <SteinsGate/Research/SGAllActorListener.h>

SGActorListenerManager::SGActorListenerManager() {}

SGActorListenerManager::~SGActorListenerManager() {

	m_hProjectileListenerMap.Values().Extension().ForEach([](SGProjectileListener* listener) {
		delete listener;
	});
}

void SGActorListenerManager::initialize() {
	m_hProjectileListenerMap.Insert(PROJECTILE_LISTENER_GUNNER_BULLET_LISTENER, new SGGunnerBulletListener);
}

SGProjectileListener* SGActorListenerManager::createProjectileListener(int projectileListenerCode) {
	DebugAssertMsg(m_hProjectileListenerMap.Exist(projectileListenerCode), "해당 코드의 프로젝틸 리스너가 존재하지 않습니다.");
	SGProjectileListener* pListener =  dynamic_cast<SGProjectileListener*>(m_hProjectileListenerMap[projectileListenerCode]->createNew());
	DebugAssertMsg(pListener, "오잉? 프로젝틸 리스너가 아닌 타입이 프로젝틸 리스너 목록에 존재합니다. 수정이 필요합니다");
	return pListener;
}
