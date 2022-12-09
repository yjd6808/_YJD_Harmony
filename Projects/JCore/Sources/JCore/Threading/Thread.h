/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 11:36:52 PM
 * =====================
 * 기존 std::thread가 마음에 좀 안드는것도 있고
 * 만들어보고 싶어서..
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Functional.h>
#include <JCore/Primitives/Atomic.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/SmartPtr.h>

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
namespace JCore {

    class Thread
    {
        using TRunnable = Action<void*>;

        struct ThreadParam
        {
            TRunnable ThreadFunc;
            Thread* Self;
            void* Param;
        };
    public:
        Thread(Thread&& other) noexcept;
        Thread(const Thread& other) = delete;

        Thread& operator=(const Thread& other) = delete;
        Thread& operator=(Thread&&) noexcept;               // 초기화 안된 경우만 이동복사 허용
    private: // 생성자로 생성을 금함
        Thread(const char* name = nullptr) : m_hHandle(nullptr), m_Name(name), m_uiThreadId(0) {}
        Thread(TRunnable&& fn, void* param = nullptr, const char* name = nullptr);

        int Start(TRunnable&& fn, void* param = nullptr);
        void Join();
        bool SetPriority(int priority);
        int GetPriority();
        Int32U GetId() { return m_uiThreadId; }
    public:
        static SharedPtr<Thread> Create(const char* name = nullptr);
        static SharedPtr<Thread> Create(TRunnable&& fn, void* param = nullptr, const char* name = nullptr);
        static Int32U GetThreadId();
    private:
        static Int32U JCoreStdCall ThreadRoutine(void* param);
    private:
        WinHandle m_hHandle;
        String m_Name;
        Int32U m_uiThreadId;
    };

    


} // namespace JCore;
