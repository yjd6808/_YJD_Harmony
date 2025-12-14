/*
 * 작성자: 윤정도
 * 생성일: 12/19/2022 5:10:28 AM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Debug/MemoryLeakDetector.h>

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
void MemoryLeakDetector::StartDetect()
{
	_CrtMemCheckpoint(&m_State);
	m_bDetecting = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
int MemoryLeakDetector::StopDetect()
{
	_CrtMemState stateNow, stateDiff;
	_CrtMemCheckpoint(&stateNow);
	_CrtMemDumpAllObjectsSince(&m_State);

	const int diffResult = _CrtMemDifference(&stateDiff, &m_State, &stateNow);
	m_bDetecting = false;

	if (diffResult)
	{
		return int(stateDiff.lSizes[_NORMAL_BLOCK]);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
AutoMemoryLeakDetector::AutoMemoryLeakDetector()
{
	_CrtMemCheckpoint(&m_State);
	m_bDetecting = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
AutoMemoryLeakDetector::AutoMemoryLeakDetector(const TAction& _action)
	: AutoMemoryLeakDetector()
{
	m_Callback = _action;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
AutoMemoryLeakDetector::AutoMemoryLeakDetector(TAction&& _action)
	: AutoMemoryLeakDetector()
{
	m_Callback = Move(_action);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
AutoMemoryLeakDetector::~AutoMemoryLeakDetector()
{
	_CrtMemState stateNow, stateDiff;
	_CrtMemCheckpoint(&stateNow);
	_CrtMemDumpAllObjectsSince(&m_State);
	const int diffResult = _CrtMemDifference(&stateDiff, &m_State, &stateNow);

	if (diffResult && m_Callback)
	{
		m_Callback(int(stateDiff.lSizes[1]));
		_CrtMemDumpStatistics(&stateDiff);
	}

	m_bDetecting = false;
}

NS_JC_END
