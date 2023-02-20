/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:25:27 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/SgaLinkSprite.h>

USING_NS_JC;

SgaLinkSprite::~SgaLinkSprite() {}

SgaLinkSpritePtr SgaLinkSprite::Create(const SgaImagePtr& parent, int frameIndex, int format) {
	return MakeShared<SgaLinkSprite>(parent, frameIndex, format);
}
