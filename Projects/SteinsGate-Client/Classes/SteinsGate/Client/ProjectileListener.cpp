/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:50:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ProjectileListener.h"

#include <SteinsGate/Client/Projectile.h>

void ProjectileListener::injectActor(Actor* actor) {
	DebugAssertMsg(actor->getType() == ActorType::Projectile, "프로젝틸 타입이 아닙니다.");
	m_pProjectile = (Projectile*)actor;
}
