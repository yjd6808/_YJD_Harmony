/*
 * 작성자: 윤정도
 * 생성일: 7/25/2023 8:34:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ActorListener.h"

#include <SteinsGate/Client/Actor.h>

void ActorListener::onCleanUp() {

	

}

ActorType_t ActorListener::getActorType() const {
	return m_pActor->getType();
}
