/*
 * 작성자: 윤정도
 * 생성일: 12/15/2022 11:39:09 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Functional.h>
#include <JCore/Pool/MemoryPoolManager.h>

namespace JCore {

	class AutMemoryPoolLeakDetector
	{
		using TOut = int*;
		using TAction = Action<Int64U, int*>;
	public:
		AutMemoryPoolLeakDetector(MemoryPoolAbstract* pool, const TAction& action);
		~AutMemoryPoolLeakDetector();
	protected:
		MemoryPoolAbstract* m_Pool;
		TAction m_Callback;
	};

	
} // namespace JCore
