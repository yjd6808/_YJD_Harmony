/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 4:29:55 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGFrameTexture.h"

#include <SteinsGate/Client/SGGlobal.h>

SGFrameTexture::~SGFrameTexture() {
}

SGSpriteFrameTexture::~SGSpriteFrameTexture() {
	DeleteSafe(m_pTexture);
}

