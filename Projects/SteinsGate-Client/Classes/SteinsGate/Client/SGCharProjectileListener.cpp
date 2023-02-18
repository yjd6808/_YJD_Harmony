/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 5:59:02 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGCharProjectileListener.h"

#include <SteinsGate/Client/GameCoreHeader.h>
#include <SteinsGate/Client/SGProjectile.h>

SGCharProjectileListener::SGCharProjectileListener()
	: m_pPlayer(nullptr)
	, m_pCharacter(nullptr)
	, m_bPlayerProjectile(false) {}

void SGCharProjectileListener::initPlayer() {
	SGActor* pSpawner = m_pProjectile->getSpawner();
	DebugAssertMsg(pSpawner->getType() == ActorType::Character, "캐릭터가 소환한 프로젝틸이 아닙니다");

	SGCharacter* pCharacter = (SGCharacter*)pSpawner;
	m_pCharacter = pCharacter;

	if (pCharacter == CorePlayer_v) {
		m_pPlayer = SGHostPlayer::get();
		m_bPlayerProjectile = true;
	}
}

bool SGCharProjectileListener::isPlayerProjectile() {
	return m_bPlayerProjectile;
}
