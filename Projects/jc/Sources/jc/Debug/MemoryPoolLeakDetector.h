/*
 * 작성자: 윤정도
 * 생성일: 12/15/2022 11:39:09 PM
 * =====================
 *
 */


#pragma once

#include <jc/Functional.h>
#include <jc/Pool/MemoryPoolAbstract.h>

NS_JC_BEGIN

class AutMemoryPoolLeakDetector
{
	using TOut = int*;
	using TAction = Action<Int64U, int*>;

public:
	AutMemoryPoolLeakDetector(MemoryPoolAbstract* _pPool, const TAction& _action);
	~AutMemoryPoolLeakDetector();

protected:
	MemoryPoolAbstract* m_Pool;
	TAction m_Callback;
};

NS_END
