/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:07:56 AM
 * =====================
 *
 */


#pragma once

#include "sg/Util/DescBase.h"

struct TileInfo : SDescBase
{
	~TileInfo() override = default;

	int sgaIndex_;
	int imgIndex_;
	int spriteIndex_;
};
