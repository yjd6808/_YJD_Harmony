/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <sg/MonsterBaseInfo.h>
#include <sgcl/Struct.h>

struct MonsterInfo : MonsterBaseInfo
{
	MonsterInfo()
	: pSpriteData_(nullptr)
	{
	}

	~MonsterInfo() override { JCORE_DELETE_SAFE(pSpriteData_); }

	ActorSpriteData* pSpriteData_;
};
