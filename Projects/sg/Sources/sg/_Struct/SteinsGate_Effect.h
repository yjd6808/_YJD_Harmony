/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include "sg/_Util/DescBase.h"

struct EffectInfo : SDescBase
{
	EffectInfo() = default;

	~EffectInfo() override
	{
		JC_DELETE_SAFE(pSpriteData_);
	}

	jc::String name_;
	ActorSpriteData* pSpriteData_;
};
