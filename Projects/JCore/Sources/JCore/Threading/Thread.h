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
    enum State
    {
        Uninitialized,
        RunningWait,
        Running,
        JoinWait,
        Joined,
        Aborted
    };

    using TRunnable = Action<void*>;

    struct ThreadParam
    {
        TRunnable ThreadFunc;
        Thread* Self;
        void* Param;
    };
public:
    Thread(const char* name = nullptr, bool autoJoin = false) : m_hHandle(nullptr), m_Name(name), m_uiThreadId(0), m_eState(Uninitialized), m_RunningSignal(1, 0), m_bAutoJoin(autoJoin) {}
    Thread(TRunnable&& fn, void* param = nullptr, const char* name = nullptr, bool autoJoin = false); 
    Thread(const Thread& other) = delete;
    Thread(Thread&& other) noexcept;
    ~Thread() noexcept;

    Thread& operator=(const Thread& other) = delete;
    Thread& operator=(Thread&&) noexcept;               // 초기화 안된 경우만 이동복사 허용
    
public:
    int Start(TRunnable&& fn, void* param = nullptr);
    void AutoJoin(bool enabled) { m_bAutoJoin = enabled; }
    bool Join();
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
    static Int32U JCoreStdCall ThreadRoutine(void* param);
private:
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
