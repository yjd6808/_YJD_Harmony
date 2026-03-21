/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:25:27 AM
 * =====================
 *
 */

#include "sg/_Sga/SgaLinkSprite.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SgaLinkSprite::~SgaLinkSprite()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaLinkSpritePtr SgaLinkSprite::Create(const SgaImagePtr& _pParent, int _frameIndex, int _format)
{
	return MakeShared<SgaLinkSprite>(_pParent, _frameIndex, _format);
}
