/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 2:28:26 PM
 * =====================
 *
 */


#include "Core.h"
#include "MapInfo.h"

#include <SteinsGate/Common/Const.h>

bool MapAreaInfo::checkWall(float x, float y) const {

	if (x < 0.0f || y < 0.0f)
		return true;

	int iX = x / Const::Map::MapAreaBlockSize;
	int iY = y / Const::Map::MapAreaBlockSize;
	int iWidth = Area[0].Length();
	int iHeight = Area.Size();

	if (iY >= iHeight)
		return true;

	if (iY < 0)
		return true;

	if (iX >= iWidth)
		return true;

	if (iX < 0)
		return true;

	return Area[iY][iX] == '1';
}

float MapAreaInfo::getAreaWidth() {
	return getAreaHorizontolBlockCount() * Const::Map::MapAreaBlockSize;
}

float MapAreaInfo::getAreaHeight() {
	return getAreaVerticalBlockCount() * Const::Map::MapAreaBlockSize;
}

