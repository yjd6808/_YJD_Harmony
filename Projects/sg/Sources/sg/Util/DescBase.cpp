/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:21:30 PM
 * =====================
 *
 */

#include "DescBase.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SgaResourceIndex::SgaResourceIndex()
	: value_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
String SgaResourceIndex::ToString() const
{
	return StringUtil::Format("Sga(%d) Img(%d) Sprite(%d)", un_.sgaIndex_, un_.imgIndex_, un_.frameIndex_);
}

