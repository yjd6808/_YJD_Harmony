/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 9:36:50 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGPlayer.h"

#include <SteinsGate/Client/SGMapLayer.h>

SGPlayer::SGPlayer()
	: m_pCharacter(nullptr)
	, m_pMapLayer(nullptr) {}


SGPlayer::~SGPlayer() {
	if (m_pCharacter) {
		DebugAssertMsg(m_pCharacter->getReferenceCount() > 0, "레퍼런스 카운트가 이미 0입니다.");
		CC_SAFE_RELEASE_NULL(m_pCharacter);
	}
}

SGActorSprite* SGPlayer::getActorSprite() {
	return m_pCharacter->getActorSprite();
}

void SGPlayer::setCharacter(SGCharacter* character) {
	CC_SAFE_RELEASE(m_pCharacter);

	m_pCharacter = character;

	if (character != nullptr) {
		m_pCharacter->retain();
		m_pCharacter->setOwner(true);
	}
}

void SGPlayer::setMapLayer(SGMapLayer* mapLayer) {
	m_pMapLayer = mapLayer;
}


SGCharacter* SGPlayer::getCharacter() {
	DebugAssertMsg(m_pCharacter, "캐릭터가 세팅되지 않았습니다.");
	return m_pCharacter;
}

SGMapLayer* SGPlayer::getMapLayer() {
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 초기화되지 않았습니다.");
	return m_pMapLayer;
}
