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

NS_JC_BEGIN

Int32U Thread::ms_uiMainThreadId = GetMainThreadId();
thread_local Int32U Thread::tls_uiThreadId = WinApi::GetCurrentThreadId();

Thread::Thread(TRunnable&& fn, void* param, const char* name, bool autoJoin): Thread(name, autoJoin) {
    Start(Move(fn), param);
}

Thread::~Thread() noexcept {
    if (m_bAutoJoin) {
        Join();
        return;
    }

    if (m_hHandle == nullptr) {
        return;
    }

    DebugAssert(!Joinable());
    WinApi::CloseHandle(m_hHandle);
}

Thread::Thread(Thread&& other) noexcept : Thread() {
    operator=(Move(other));
}

bool Thread::SetPriority(int priority) {

    return WinApi::SetThreadPriority(m_hHandle, priority);
}

int Thread::GetPriority() {
    return WinApi::GetThreadPriority(m_hHandle);
}

Int32U Thread::GetId() {
    
    if (m_eState >= Running)
        return m_uiThreadId;

    return 0;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    m_hHandle = other.m_hHandle;
    m_uiThreadId = other.m_uiThreadId;
    m_Name = Move(other.m_Name);
    m_bAutoJoin = other.m_bAutoJoin;
    m_eState = other.m_eState;

    other.m_hHandle = nullptr;
    other.m_uiThreadId = 0;
    other.m_eState = Uninitialized;
    other.m_bAutoJoin = false;

    return *this;
}

Int32U Thread::GetThreadId() {
    if (tls_uiThreadId != 0)
        return tls_uiThreadId;

    return WinApi::GetCurrentThreadId();
}

void Thread::Sleep(Int32U ms) {
    return ::Sleep(ms);
}

Int32U JCoreStdCall Thread::ThreadRoutine(void* param) {
    auto* pRecvParam = static_cast<ThreadParam*>(param);
    Thread* pThis = pRecvParam->Self;
    TRunnable runnable = Move(pRecvParam->ThreadFunc);
    void* pParam = pRecvParam->Param;

    // ↑ 시그널 주기전에 미리 매개변수들 가져와야함 (pRecvParam은 이제 사용하지 말 것)
    pThis->m_uiThreadId = Thread::GetThreadId();
    pThis->m_RunningSignal.Release();
    runnable(pParam);
    pThis->m_eState = JoinWait;
    
    CRuntime::EndThreadEx(0);
    return 0;
}

int Thread::Start(TRunnable&& fn, void* param) {
    DebugAssertMsg(m_eState == Uninitialized, "이미 시작된적이 있는 쓰레드입니다.");      // 재시작 막음
    m_eState = RunningWait;
    SharedPtr<ThreadParam> spSend = MakeShared<ThreadParam>();
    spSend->Param = param;
    spSend->Self = this;
    spSend->ThreadFunc = Move(fn);

    m_hHandle = reinterpret_cast<WinHandle>(CRuntime::BeginThreadEx(ThreadRoutine, spSend.GetPtr()));
    
    if (m_hHandle == NULL) {
        m_eState = Aborted;
        return CRuntime::ErrorNo();
    }

    m_RunningSignal.Acquire();
    m_eState = Running;

    return 0;
}

bool Thread::Join() {
    int state = m_eState;

    if (state == Uninitialized || state == Aborted)
        return false;

    if (state == Joined)
        return true;

    Int32U iWaitResult = WinApi::WaitForMultipleObjectsEx(1, &m_hHandle, true);

    if (iWaitResult != WAIT_OBJECT_0) {
        std::terminate();
    }

    WinApi::CloseHandle(m_hHandle);
    m_hHandle = nullptr;
    m_eState = Joined;
    return true;
}

bool Thread::Joinable() {
    int state = m_eState;
    return state >= RunningWait && state <= JoinWait;
}

void Thread::Abort() {
    WinApi::CloseHandle(m_hHandle);
    m_eState = Aborted;
}

NS_JC_END




