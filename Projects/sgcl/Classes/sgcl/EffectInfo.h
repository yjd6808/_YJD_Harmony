/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>
#include <sgcl/Struct.h>

struct EffectInfo : SDescBase
{
	EffectInfo() = default;

	~EffectInfo() override
	{
		JCORE_DELETE_SAFE(pSpriteData_);
	}

	jc::String name_;
	ActorSpriteData* pSpriteData_;
};
