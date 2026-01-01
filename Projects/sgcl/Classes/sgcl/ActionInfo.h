/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:04:49 AM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>
#include <sgcl/ComboKeyList.h>

struct ActionInfo : SDescBase
{
	~ActionInfo() override = default;

	jc::String ActionName;
	bool ForceCancelable;
	bool Moveable;
	float SpeedX;
	float SpeedY;
	ComboKeyList Command;
};
