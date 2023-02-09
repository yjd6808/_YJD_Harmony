/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 11:47:01 AM
 * =====================
 *
 */


#pragma once

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW


#include <crtdbg.h>
#include <JCore/Type.h>
#include <JCore/TypeCast.h>
#include <JCore/Functional.h>


NS_JC_BEGIN


    // 범위 메모리릭 체크
    // @코드 획득 주소 : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
    class MemoryLeakDetector
    {
    public:
        void StartDetect();
        bool Detecting() { return m_bDetecting; }
        int StopDetect();
    protected:
        _CrtMemState m_State{};
        bool m_bDetecting{};
    };

	class AutoMemoryLeakDetector : public MemoryLeakDetector
    {
        using TAction = Action<Int32U>;
    public:
        AutoMemoryLeakDetector();
        AutoMemoryLeakDetector(const TAction& action);
        AutoMemoryLeakDetector(TAction&& action);
        ~AutoMemoryLeakDetector();
    private:
        TAction m_Callback;
    };
}



