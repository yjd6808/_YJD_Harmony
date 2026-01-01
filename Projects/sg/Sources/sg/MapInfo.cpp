/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 2:28:26 PM
 * =====================
 *
 */


#include "Core.h"
#include "MapInfo.h"

#include <sg/Const.h>

//////////////////////////////////////////////////////////////////////////////////////////
bool MapAreaInfo::CheckWall(float _x, float _y) const
{
	if (_x < 0.0f || _y < 0.0f)
		return true;

	int blockX = _x / Const::Map::MapAreaBlockSize;
	int blockY = _y / Const::Map::MapAreaBlockSize;
	int width = area_[0].Length();
	int height = area_.Size();

	if (blockY >= height)
		return true;

	if (blockY < 0)
		return true;

	if (blockX >= width)
		return true;

	if (blockX < 0)
		return true;

	return area_[blockY][blockX] == '1';
}

//////////////////////////////////////////////////////////////////////////////////////////
float MapAreaInfo::GetAreaWidth()
{
	return GetAreaHorizontolBlockCount() * Const::Map::MapAreaBlockSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
float MapAreaInfo::GetAreaHeight()
{
	return GetAreaVerticalBlockCount() * Const::Map::MapAreaBlockSize;
}
