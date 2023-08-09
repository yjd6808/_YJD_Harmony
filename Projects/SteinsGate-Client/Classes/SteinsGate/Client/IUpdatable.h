/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:15:52 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>

class JCORE_NOVTABLE IUpdatable
{
public:
	virtual ~IUpdatable() = default;
	virtual void onUpdate(float dt) = 0;
};
