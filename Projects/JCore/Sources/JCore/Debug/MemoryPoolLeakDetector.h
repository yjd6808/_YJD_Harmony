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
		AutMemoryPoolLeakDetector(MemoryPoolManager* manager, const TAction& action) : m_PoolManager(manager), m_Callback(action) {
			m_PoolManager->StartDetectLeak();
		}

		~AutMemoryPoolLeakDetector() {
			TOut detail;
			Int64U uiTotalLeak = m_PoolManager->StopDetectLeak(&detail);

			if (m_Callback) {
				m_Callback(uiTotalLeak, detail);
			}
		}

	protected:
		MemoryPoolManager* m_PoolManager;
		TAction m_Callback;
	};

	
} // namespace JCore
