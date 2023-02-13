/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:25:27 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/NpkLinkSprite.h>

using namespace JCore;

NpkLinkSprite::~NpkLinkSprite() {}

NpkLinkSpritePtr NpkLinkSprite::Create(const NpkImagePtr& parent, int frameIndex, int format) {
	return MakeShared<NpkLinkSprite>(parent, frameIndex, format);
}
