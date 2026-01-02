/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Struct/SteinsGate_MonsterBase.h>

struct MonsterInfo : MonsterBaseInfo
{
	MonsterInfo()
	: pSpriteData_(nullptr)
	{
	}

	~MonsterInfo() override { JC_DELETE_SAFE(pSpriteData_); }

	ActorSpriteData* pSpriteData_;
};
