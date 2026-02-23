/*
 * 작성자: 윤정도
 * 생성일: 12/19/2022 5:14:33 AM
 * =====================
 *
 */


#include <jc/Core.h>
#include <jc/Debug/MemoryPoolLeakDetector.h>

#include <crtdbg.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
AutMemoryPoolLeakDetector::AutMemoryPoolLeakDetector(MemoryPoolAbstract* _pPool, const TAction& _action)
	: m_Pool(_pPool)
	, m_Callback(_action)
{
	m_Pool->StartDetectLeak();
}

//////////////////////////////////////////////////////////////////////////////////////////
AutMemoryPoolLeakDetector::~AutMemoryPoolLeakDetector()
{
	int detail[detail::MemoryBlockSizeMapSize_v];
	_u64 totalLeak = m_Pool->StopDetectLeak(detail);

	if (m_Callback)
	{
		m_Callback(totalLeak, detail);
	}
}

NS_END
