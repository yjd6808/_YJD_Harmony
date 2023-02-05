/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:50:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGProjectileListener.h"

#include <SteinsGate/Research/SGProjectile.h>

void SGProjectileListener::injectActor(SGActor* actor) {
	DebugAssertMsg(actor->getType() == ActorType::Projectile, "프로젝틸 타입이 아닙니다.");
	m_pProjectile = (SGProjectile*)actor;
}
