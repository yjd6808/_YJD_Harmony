/*
 * 작성자: 윤정도
 * 생성일: 12/19/2022 5:14:33 AM
 * =====================
 *
 */



#include <JCore/Core.h>
#include <JCore/Debug/MemoryPoolLeakDetector.h>

#include <crtdbg.h>

namespace JCore {

	AutMemoryPoolLeakDetector::AutMemoryPoolLeakDetector(MemoryPoolManager* manager, const TAction& action)
	: m_PoolManager(manager)
	, m_Callback(action) {
		m_PoolManager->StartDetectLeak();
	}

	AutMemoryPoolLeakDetector::~AutMemoryPoolLeakDetector() {
		TOut detail;
		Int64U uiTotalLeak = m_PoolManager->StopDetectLeak(&detail);

		if (m_Callback) {
			m_Callback(uiTotalLeak, detail);
		}
	}
}
