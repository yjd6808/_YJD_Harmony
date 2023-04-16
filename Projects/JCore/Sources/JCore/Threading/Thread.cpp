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

Int32U Thread::ms_uiMainThreadId = WinApi::GetCurrentThreadId();
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
    
    if (m_eState >= eRunning)
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
    other.m_eState = eUninitialized;
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
    pThis->m_eState = eJoinWait;
    
    CRuntime::EndThreadEx(0);
    return 0;
}

int Thread::Start(TRunnable&& fn, void* param) {
    DebugAssertMsg(m_eState == eUninitialized, "이미 시작된적이 있는 쓰레드입니다.");      // 재시작 막음
    m_eState = eRunningWait;
    SharedPtr<ThreadParam> spSend = MakeShared<ThreadParam>();
    spSend->Param = param;
    spSend->Self = this;
    spSend->ThreadFunc = Move(fn);

    m_hHandle = reinterpret_cast<WinHandle>(CRuntime::BeginThreadEx(ThreadRoutine, spSend.GetPtr()));
    
    if (m_hHandle == NULL) {
        m_eState = eAborted;
        return CRuntime::ErrorNo();
    }

    m_RunningSignal.Acquire();
    m_eState = eRunning;

    return 0;
}

Thread::JoinResult Thread::Join(int timeoutMiliSeconds) {
    const int state = m_eState;

    if (state == eUninitialized || state == eAborted)
        return eNotJoinable;

    if (state == eJoined)
        return eAlreadyJoined;

    const Int32U iWaitResult = WinApi::WaitForMultipleObjectsEx(1, &m_hHandle, true, timeoutMiliSeconds);

    if (iWaitResult == WAIT_TIMEOUT) {
        return eTimeout;
    }

	// https://learn.microsoft.com/ko-kr/windows/win32/sync/waiting-for-multiple-objects
    if (iWaitResult != WAIT_OBJECT_0) {
        // ::GetLastError();
        return eError;
    }

    WinApi::CloseHandle(m_hHandle);
    m_hHandle = nullptr;
    m_eState = eJoined;
    return eSuccess;
}

bool Thread::Joinable() {
    const int state = m_eState;
    return state >= eRunningWait && state <= eJoinWait;
}

void Thread::Abort() {
    WinApi::CloseHandle(m_hHandle);
    m_hHandle = nullptr;
    m_eState = eAborted;
}

NS_JC_END




