/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 8:11:16 PM
 * =====================
 *
 */

#include "jc/TypeCast.h"
#include "jc/Threading/Thread.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/Wrapper/WinApi.h"

NS_JC_BEGIN

_u32 Thread::ms_uiMainThreadId = WinApi::GetCurrentThreadId();
thread_local _u32 Thread::tls_uiThreadId = 0;

//////////////////////////////////////////////////////////////////////////////////////////
Thread::Thread(TRunnable&& _fn, void* _pParam, const char* _pName, bool _autoJoin)
: Thread(_pName, _autoJoin)
{
    Start(Move(_fn), _pParam);
}

//////////////////////////////////////////////////////////////////////////////////////////
Thread::~Thread() noexcept
{
    if (m_bAutoJoin)
    {
        Join();
        return;
    }

    if (m_hHandle == nullptr)
    {
        return;
    }

    jc_assert(!Joinable());
    WinApi::CloseHandle(m_hHandle);
}

//////////////////////////////////////////////////////////////////////////////////////////
Thread::Thread(Thread&& _other) noexcept
	: Thread()
{
    operator=(Move(_other));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Thread::SetPriority(int _priority)
{
    return WinApi::SetThreadPriority(m_hHandle, _priority);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Thread::GetPriority()
{
    return WinApi::GetThreadPriority(m_hHandle);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 Thread::GetId()
{
    if (m_eState >= eRunning)
        return m_uiThreadId;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
Thread& Thread::operator=(Thread&& _other) noexcept
{
    m_hHandle = _other.m_hHandle;
    m_uiThreadId = _other.m_uiThreadId;
    m_Name = Move(_other.m_Name);
    m_bAutoJoin = _other.m_bAutoJoin;
    m_eState = _other.m_eState;

    _other.m_hHandle = nullptr;
    _other.m_uiThreadId = 0;
    _other.m_eState = eUninitialized;
    _other.m_bAutoJoin = false;

    return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 Thread::GetThreadId()
{
    if (tls_uiThreadId != 0)
        return tls_uiThreadId;

    return tls_uiThreadId = WinApi::GetCurrentThreadId();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Thread::Sleep(_u32 _milliseconds)
{
    ::Sleep(_milliseconds);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 JC_STDCALL Thread::ThreadRoutine(void* _pParam)
{
    {
        auto* pRecvParam = static_cast<ThreadParam*>(_pParam);
		{
			Thread* pThis = pRecvParam->Self;
			TRunnable runnable = Move(pRecvParam->ThreadFunc);
			void* pParam = pRecvParam->Param;

			pThis->m_uiThreadId = Thread::GetThreadId();
			pThis->m_RunningSignal.Release();
			runnable(pParam);
			pThis->m_eState = eJoinWait;
		}

        delete pRecvParam;
    }

	CRuntime::EndThreadEx(0);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Thread::Start(TRunnable&& _fn, void* _pParam)
{
    jc_assert_msg(m_eState == eUninitialized, "이미 시작된적이 있는 쓰레드입니다.");      // 재시작 막음
    m_eState = eRunningWait;
    ThreadParam* pStartParam = dbg_new ThreadParam;
    pStartParam->Param = _pParam;
    pStartParam->Self = this;
    pStartParam->ThreadFunc = Move(_fn);

    m_hHandle = reinterpret_cast<_whandle>(CRuntime::BeginThreadEx(ThreadRoutine, pStartParam));
    
    if (m_hHandle == NULL)
    {
        m_eState = eAborted;
        delete pStartParam;
        return CRuntime::ErrorNo();
    }

    m_RunningSignal.Acquire();
    m_eState = eRunning;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
Thread::JoinResult Thread::Join(int _timeoutMiliSeconds)
{
    const int state = m_eState;

    if (state == eUninitialized || state == eAborted)
        return eNotJoinable;

    if (state == eJoined)
        return eAlreadyJoined;

    const _u32 waitResult = WinApi::WaitForSingleObject(m_hHandle, _timeoutMiliSeconds);

    if (waitResult == WAIT_TIMEOUT)
    {
        return eTimeout;
    }

	// https://learn.microsoft.com/ko-kr/windows/win32/sync/waiting-for-multiple-objects
    if (waitResult != WAIT_OBJECT_0)
    {
        // ::GetLastError();
        return eError;
    }

    WinApi::CloseHandle(m_hHandle);
    m_hHandle = nullptr;
    m_eState = eJoined;
    return eSuccess;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Thread::Joinable()
{
    const int state = m_eState;
    return state >= eRunningWait && state <= eJoinWait;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Thread::Abort() // detach랑 같은 효과
{
    WinApi::CloseHandle(m_hHandle); // detach
    m_hHandle = nullptr;
    m_eState = eAborted;
}

NS_END




