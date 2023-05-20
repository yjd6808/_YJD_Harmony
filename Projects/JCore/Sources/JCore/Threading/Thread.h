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
#include <JCore/Event.h>
#include <JCore/Sync/Semaphore.h>
#include <JCore/Primitives/Atomic.h>
#include <JCore/Primitives/String.h>

NS_JC_BEGIN

class Thread
{
public:
    enum State
    {
        eUninitialized,
        eRunningWait,
        eRunning,
        eJoinWait,
        eJoined,
        eAborted
    };

    enum JoinResult
    {
        eSuccess,
        eNotJoinable,       // 쓰레드가 Join가능한 상태가 아닌 경우
        eAlreadyJoined,     // 이미 Join된 경우
        eTimeout,           // 타임아웃된 경우
        eError              // Join 시도중 오류가 발생한 경우(GetLastError로 오류코드 확인할 것)
    };

    using TRunnable = Action<void*>;

    struct ThreadParam
    {
        TRunnable ThreadFunc;
        Thread* Self;
        void* Param;
    };

    Thread(const char* name = nullptr, bool autoJoin = false) : m_hHandle(nullptr), m_Name(name), m_uiThreadId(0), m_eState(eUninitialized), m_RunningSignal(1, 0), m_bAutoJoin(autoJoin) {}
    Thread(TRunnable&& fn, void* param = nullptr, const char* name = nullptr, bool autoJoin = false); 
    Thread(const Thread& other) = delete;
    Thread(Thread&& other) noexcept;
    virtual ~Thread() noexcept;

    Thread& operator=(const Thread& other) = delete;
    Thread& operator=(Thread&&) noexcept;               // 초기화 안된 경우만 이동복사 허용
    
public:
    int Start(TRunnable&& fn, void* param = nullptr);
    void AutoJoin(bool enabled) { m_bAutoJoin = enabled; }
    JoinResult Join(int timeoutMiliSeconds = JCORE_INFINITE);
    bool Joinable();
    void Abort();
    bool SetPriority(int priority);
    int GetPriority();
    int GetState() { return m_eState; }
    Int32U GetId();

    static Int32U GetThreadId();
    static Int32U GetMainThreadId() { return ms_uiMainThreadId; }
    static bool IsMainThread() { return ms_uiMainThreadId == GetThreadId(); }
	static void Sleep(Int32U ms);
private:
    static Int32U JCORE_STDCALL ThreadRoutine(void* param);
protected:
    WinHandle m_hHandle;
    String m_Name;
    Int32U m_uiThreadId;
    AtomicInt m_eState;
    Semaphore m_RunningSignal;
    bool m_bAutoJoin;

    static Int32U ms_uiMainThreadId;
    static thread_local Int32U tls_uiThreadId;
};



NS_JC_END;
