/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:07:56 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct TileInfo : ConfigDataAbstract
{
	~TileInfo() override = default;

	int SgaIndex;
	int ImgIndex;
	int SpriteIndex;
};