/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 2:28:26 PM
 * =====================
 *
 */


#include "SGMapInfo.h"

#include <SteinsGate/Research/SGStruct.h>


bool SGMapInfo::checkWall(float x, float y) {

	int iX = x / BlockSize_v;
	int iY = y / BlockSize_v;
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
