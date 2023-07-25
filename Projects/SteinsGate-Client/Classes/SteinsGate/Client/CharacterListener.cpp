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
	ActorListener::onCleanUp();

	if (m_pActor->hasCleanUpFlag(Actor::CF_ReleaseActorSprite)) {
		m_pActor->releaseActorSprite();
	}
}

void CharacterListener::injectActor(Actor* actor) {
	DebugAssertMsg(actor->getType() == ActorType::Character, "캐릭터 타입이 아닙니다.");
	m_pActor = actor;
	m_pCharacter = dynamic_cast<Character*>(actor);
}
