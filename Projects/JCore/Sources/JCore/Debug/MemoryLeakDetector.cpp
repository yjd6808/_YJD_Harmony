/*
 * 작성자: 윤정도
 * 생성일: 12/19/2022 5:10:28 AM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Debug/MemoryLeakDetector.h>

NS_JC_BEGIN

    // ===================================================================================================================
    //                                              MemoryLeakDetector
    // ===================================================================================================================
    void MemoryLeakDetector::StartDetect() {
        _CrtMemCheckpoint(&m_State);
        m_bDetecting = true;
    }

	int MemoryLeakDetector::StopDetect() {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);

        const int diffResult = _CrtMemDifference(&stateDiff, &m_State, &stateNow);
        m_bDetecting = false;
        if (diffResult) {
            return stateDiff.lSizes[1];
        }
        return 0;
    }


    // ===================================================================================================================
    //                                              AutoMemoryLeakDetector
    // ===================================================================================================================
	AutoMemoryLeakDetector::AutoMemoryLeakDetector() {
        _CrtMemCheckpoint(&m_State);
        m_bDetecting = true;
    }

	AutoMemoryLeakDetector::AutoMemoryLeakDetector(const TAction& action) : AutoMemoryLeakDetector() {
        m_Callback = action;
    }

	AutoMemoryLeakDetector::AutoMemoryLeakDetector(TAction&& action) : AutoMemoryLeakDetector() {
        m_Callback = Move(action);
    }

	AutoMemoryLeakDetector::~AutoMemoryLeakDetector() {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);
        const int diffResult = _CrtMemDifference(&stateDiff, &m_State, &stateNow);

        if (diffResult && m_Callback) {
            m_Callback(stateDiff.lSizes[1]);
            _CrtMemDumpStatistics(&stateDiff);
        }

        m_bDetecting = false;
    }
}

