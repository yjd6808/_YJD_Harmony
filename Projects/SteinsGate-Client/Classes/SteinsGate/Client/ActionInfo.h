/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:04:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Client/ComboKeyList.h>

struct ActionInfo : ConfigDataAbstract
{
	~ActionInfo() override = default;

	SGString ActionName;
	bool ForceCancelable;
	bool Moveable;
	float SpeedX;
	float SpeedY;
	ComboKeyList Command;
};