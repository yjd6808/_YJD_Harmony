/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 4:29:55 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "FrameTexture.h"

#include <SteinsGate/Client/Global.h>

FrameTexture::~FrameTexture() {
}

SpriteFrameTexture::~SpriteFrameTexture() {
	DeleteSafe(m_pTexture);
}

