/*
 * 작성자: 윤정도
 * 생성일: 12/19/2022 5:14:33 AM
 * =====================
 *
 */



#include <JCore/Core.h>
#include <JCore/Debug/MemoryPoolLeakDetector.h>

#include <crtdbg.h>

NS_JC_BEGIN

	AutMemoryPoolLeakDetector::AutMemoryPoolLeakDetector(MemoryPoolAbstract* pool, const TAction& action)
	: m_Pool(pool)
	, m_Callback(action) {
		m_Pool->StartDetectLeak();
	}

	AutMemoryPoolLeakDetector::~AutMemoryPoolLeakDetector() {
		int detail[Detail::MemoryBlockSizeMapSize_v];
		Int64U uiTotalLeak = m_Pool->StopDetectLeak(detail);

		if (m_Callback) {
			m_Callback(uiTotalLeak, detail);
		}
	}
}
