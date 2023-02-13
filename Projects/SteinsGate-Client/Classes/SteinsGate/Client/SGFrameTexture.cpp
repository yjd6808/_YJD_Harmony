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
}


SGSpriteFrameTexture* SGSpriteFrameTexture::createDefaultTextureRetain() {
	SGTexture* pDefault = SGGlobal::get()->getDefaultTexture();
	SGSpriteFrameTexture* pFrameTexture = new SGSpriteFrameTexture(pDefault, { 0, 0, 2, 2, 2, 2 }, 0, true);
	pFrameTexture->autorelease();
	pFrameTexture->retain();
	return pFrameTexture;
}

