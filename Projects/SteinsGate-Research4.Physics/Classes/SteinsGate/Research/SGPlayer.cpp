/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */


#include "SGPlayer.h"

SGPlayer::SGPlayer()
	: m_pCharacter(nullptr) {
	
}

SGPlayer::~SGPlayer() {
	CC_SAFE_RELEASE(m_pCharacter);
}
