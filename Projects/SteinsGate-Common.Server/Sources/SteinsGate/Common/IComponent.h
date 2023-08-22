/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:21:46 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>

struct JCORE_NOVTABLE IComponent
{
	virtual ~IComponent() = default;
	virtual void Initialize() = 0;
};


