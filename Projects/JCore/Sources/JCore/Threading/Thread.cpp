/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 8:11:16 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/TypeCast.h>
#include <JCore/Threading/Thread.h>
#include <JCore/Wrapper/CRuntime.h>
#include <JCore/Wrapper/WinApi.h>

namespace JCore {

    Thread::Thread(TRunnable&& fn, void* param, const char* name): Thread(name) {
        Start(Move(fn), param);
    }

    Thread::Thread(Thread&& other) noexcept {
        operator=(Move(other));
    }

    bool Thread::SetPriority(int priority) {
        //std::make_shared<Thread>();
        return WinApi::SetThreadPriority(m_hHandle, priority);
    }

    int Thread::GetPriority() {
        return WinApi::GetThreadPriority(m_hHandle);
    }

    Thread& Thread::operator=(Thread&& other) noexcept {
        DebugAssertMessage(m_hHandle == nullptr, "현재 쓰레드가 동작중입니다.");

        m_hHandle = other.m_hHandle;
        m_uiThreadId = other.m_uiThreadId;
        m_Name = Move(other.m_Name);
        return *this;
    }

    SharedPtr<Thread> Thread::Create(const char* name) {
        return SharedPtr<Thread> { new Thread { name } };
    }
    
    SharedPtr<Thread> Thread::Create(TRunnable&& fn, void* param, const char* name) {
        return SharedPtr<Thread> { new Thread{ Move(fn), param, name } };
    }

    Int32U Thread::GetThreadId() {
        return WinApi::GetCurrentThreadId();
    }

    Int32U JCoreStdCall Thread::ThreadRoutine(void* param) {
        auto* pRecv = static_cast<ThreadParam*>(param);
        pRecv->Self->m_uiThreadId = Thread::GetThreadId();
        pRecv->ThreadFunc(pRecv->Param);

        CRuntime::EndThreadEx(0);
        return 0;
    }

    int Thread::Start(TRunnable&& fn, void* param) {
        SharedPtr<ThreadParam> spSend = MakeShared<ThreadParam>();
        spSend->Param = param;
        spSend->Self = this;
        spSend->ThreadFunc = Move(fn);

        m_hHandle = reinterpret_cast<WinHandle>(CRuntime::BeginThreadEx(ThreadRoutine, spSend.GetPtr()));

        if (m_hHandle == NULL) {
            return 0;
        }

        return CRuntime::ErrorNo();
    }

    void Thread::Join() {
        WinApi::WaitForMultipleObjectsEx(1, &m_hHandle, true);
    }
} // namespace JCore




