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
		using TOut = LinkedList<SharedPtr<MemoryPoolCaptured>>;
		using TAction = Action<Int64U, TOut&>;
	public:
		AutMemoryPoolLeakDetector(MemoryPoolManager* manager, const TAction& action);
		~AutMemoryPoolLeakDetector();
	protected:
		MemoryPoolManager* m_PoolManager;
		TAction m_Callback;
	};

	
} // namespace JCore
