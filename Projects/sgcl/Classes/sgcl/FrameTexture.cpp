/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 4:29:55 AM
 * =====================
 *
 */

#include "Core.h"
#include "FrameTexture.h"

#include <sgcl/Global.h>

FrameTexture::~FrameTexture() {
}

SpriteFrameTexture::~SpriteFrameTexture() {
	_LogDebug_("%d 삭제", frameIndex_);
	JC_DELETE_SAFE(pTexture_);
}

