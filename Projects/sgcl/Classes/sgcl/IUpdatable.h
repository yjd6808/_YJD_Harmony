/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:15:52 AM
 * =====================
 *
 */


#pragma once

#include <jc/Declspec.h>

class JC_NOVTABLE IUpdatable
{
public:
	virtual ~IUpdatable() = default;
	virtual void OnUpdate(float _dt) = 0;
};
