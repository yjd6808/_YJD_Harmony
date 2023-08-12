/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "CharacterListener.h"

void CharacterListener::onCleanUp() {
	IActorListener::onCleanUp();

	if (m_pCharacter->hasCleanUpFlag(Actor::CF_ReleaseActorSprite)) {
		m_pCharacter->releaseActorSprite();
	}
}