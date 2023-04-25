﻿/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 2:28:26 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "MapInfo.h"

#include <SteinsGate/Client/Struct.h>


bool MapInfo::checkWall(float x, float y) {

	if (x < 0.0f || y < 0.0f)
		return true;

	int iX = x / BlockSize_v;
	int iY = y / BlockSize_v;
	int iWidth = Area[0].Length();
	int iHeight = Area.Size();

	// 컴팔러가 알아서 최적화하겠지.
	// 조건 들어오는거 쉽게 확인할려고 일부러 떼어놓음
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

bool MapInfo::checkWall(const SGVec2& pos) {
	return checkWall(pos.x, pos.y);
}