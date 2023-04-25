/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 5:59:02 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "CharProjectileListener.h"

#include <SteinsGate/Client/GameCoreHeader.h>
#include <SteinsGate/Client/Projectile.h>

CharProjectileListener::CharProjectileListener()
	: m_pPlayer(nullptr)
	, m_pCharacter(nullptr)
	, m_bPlayerProjectile(false) {}

void CharProjectileListener::initPlayer() {
	Actor* pSpawner = m_pProjectile->getSpawner();
	DebugAssertMsg(pSpawner->getType() == ActorType::Character, "캐릭터가 소환한 프로젝틸이 아닙니다");

	Character* pCharacter = (Character*)pSpawner;
	m_pCharacter = pCharacter;

	if (pCharacter == CorePlayer_v) {
		m_pPlayer = CorePlayer_v;
		m_bPlayerProjectile = true;
	}
}

bool CharProjectileListener::isPlayerProjectile() {
	return m_bPlayerProjectile;
}
