/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 11:47:01 AM
 * =====================
 *
 */


#pragma once


#include <crtdbg.h>
#include <JCore/Type.h>
#include <JCore/TypeCast.h>
#include <JCore/Functional.h>




namespace JCore {


    // 범위 메모리릭 체크
    // @코드 획득 주소 : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
    class MemoryLeakDetector
    {
    public:
        void StartDetect() {
            _CrtMemCheckpoint(&m_State);
            m_bDetecting = true;
        }

        bool Detecting() {
            return m_bDetecting;
        }

        int StopDetect() {
            _CrtMemState stateNow, stateDiff;
            _CrtMemCheckpoint(&stateNow);

            const int diffResult = _CrtMemDifference(&stateDiff, &m_State, &stateNow);
            m_bDetecting = false;
            if (diffResult) {
                return stateDiff.lSizes[1];
            }

            return 0;
        }
    protected:
        _CrtMemState m_State{};
        bool m_bDetecting{};
    };

    
    class AutoMemoryLeakDetector : public MemoryLeakDetector
    {
        using TAction = Action<Int32U>;
    public:
        AutoMemoryLeakDetector() {
	        _CrtMemCheckpoint(&m_State);
        	m_bDetecting = true;
        }
        AutoMemoryLeakDetector(const TAction& action) : AutoMemoryLeakDetector() {
            m_Callback = action;
        }

        AutoMemoryLeakDetector(TAction&& action) : AutoMemoryLeakDetector() {
            m_Callback = Move(action);
        }

        ~AutoMemoryLeakDetector() {
            _CrtMemState stateNow, stateDiff;
            _CrtMemCheckpoint(&stateNow);
            const int diffResult = _CrtMemDifference(&stateDiff, &m_State, &stateNow);

            if (diffResult && m_Callback) {
                m_Callback(stateDiff.lSizes[1]);
                _CrtMemDumpStatistics(&stateDiff);
            }

            m_bDetecting = false;
        }
    private:
        TAction m_Callback;
    };
}



