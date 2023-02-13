/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:04:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGComboKeyList.h>

struct SGActionInfo
{
	int Code;
	SGString ActionName;
	bool ForceCancelable;
	bool Moveable;
	float SpeedX;
	float SpeedY;
	SGComboKeyList Command;
};