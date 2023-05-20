/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:46 PM
 * =====================
 * 윈도우 헤더파일은 너무 묵직해서 진짜 필요한 경우가 아니면
 * 내가 만든 헤더파일에는 포함시키기 싫다. 그래서 따로 필요한 기능만 빼줌
 * 사실 Windows.h라는 글자자체가 헤더파일에 있는게 좀 보기 그렇다.
 * 기능별 설명은 마이크로소프트 홈페이지꺼 복사해서 붙여넣음
 * 매번 웹사이트로가서 설명읽기 귀찮다.
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>
#include <JCore/TypeTraits.h>


NS_JC_BEGIN

struct WinApi {
	#pragma region Win32 Struct
    struct ListEntry {
        ListEntry* Flink;
        ListEntry* Blink;
    };

    struct CriticalSection;
    struct CriticalSectionDebug {
        Int16       Type;
        Int16       CreatorBackTraceIndex;
        CriticalSection* CriticalSection;
        ListEntry   ProcessLocksList;
        Int32UL     EntryCount;
        Int32UL     ContentionCount;
        Int32UL     Flags;
        Int16       CreatorBackTraceIndexHigh;
        Int16       SpareWORD;
    };
	#pragma pack(push, 8)
    struct CriticalSection {
        CriticalSectionDebug DebugInfo;

        //
        //  The following three fields control entering and exiting the critical
        //  section for the resource
        //

        Int32L LockCount;
        Int32L RecursionCount;
        WinHandle OwningThread;        // from the thread's ClientId->UniqueThread
        WinHandle LockSemaphore;
        IntPtr SpinCount;            // force size on 64-bit systems when packed
    };
	#pragma pack(pop)
	#pragma endregion

    static WinHandle InvalidHandleValue;

    static bool            JCORE_STDCALL SetConsoleCursorPosition(JCORE_IN WinHandle hStdoutHandle, JCORE_IN int x, JCORE_IN int y);
    static bool            JCORE_STDCALL GetConsoleCursorPosition(JCORE_IN WinHandle hStdoutHandle, JCORE_OUT int& x, JCORE_OUT int& y);
    static bool            JCORE_STDCALL SetConsoleTextAttribute(JCORE_IN WinHandle hStdoutHandle, JCORE_IN Int16 attribute);
    static bool            JCORE_STDCALL SetConsoleOutputCodePage(JCORE_IN Int32 codePage);
    static Int             JCORE_STDCALL GetConsoleOutputCodePage();


    /** https://learn.microsoft.com/en-us/windows/console/getstdhandle
     * \brief Retrieves a handle to the specified standard device
     * \return The standard output device. Initially, this is the active console screen buffer,
     */
    static WinHandle       JCORE_STDCALL GetStdoutHandle();



    /** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventa
     * \brief Creates or opens a named or unnamed event object.
     * \param initialState If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
     * \param manualReset If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled. If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released.
     * \param name The name of the event object. The name is limited to MAX_PATH characters. Name comparison is case sensitive.
     * \return If the function succeeds, the return value is a handle to the event object. If the named event object existed before the function call, the function returns a handle to the existing object and GetLastError returns ERROR_ALREADY_EXISTS.
     *         If the function fails, the return value is NULL. To get extended error information, call GetLastError.
     */
    static WinHandle       JCORE_STDCALL CreateEventA(JCORE_IN bool initialState, JCORE_IN bool manualReset, JCORE_IN_OPT const char* name = nullptr);

    /** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitformultipleobjectsex
     * \brief Waits until one or all of the specified objects are in the signaled state, an I/O completion routine or asynchronous procedure call (APC) is queued to the thread, or the time-out interval elapses.
     * \param eventCount The number of object handles to wait for in the array pointed to by lpHandles. The maximum number of object handles is MAXIMUM_WAIT_OBJECTS. This parameter cannot be zero.
     * \param handles An array of object handles. For a list of the object types whose handles can be specified, see the following Remarks section. The array can contain handles of objects of different types. It may not contain multiple copies of the same handle.
     * \param waitAll If this parameter is TRUE, the function returns when the state of all objects in the lpHandles array is set to signaled. If FALSE, the function returns when the state of any one of the objects is set to signaled. In the latter case, the return value indicates the object whose state caused the function to return.
     * \param timeout The time-out interval, in milliseconds. If a nonzero value is specified, the function waits until the specified objects are signaled, an I/O completion routine or APC is queued, or the interval elapses. If dwMilliseconds is zero, the function does not enter a wait state if the criteria is not met; it always returns immediately. If dwMilliseconds is INFINITE, the function will return only when the specified objects are signaled or an I/O completion routine or APC is queued.
     * \param alertable If this parameter is TRUE and the thread is in the waiting state, the function returns when the system queues an I/O completion routine or APC, and the thread runs the routine or function. Otherwise, the function does not return and the completion routine or APC function is not executed.
     * \return If the function succeeds, the return value indicates the event that caused the function to return. It can be one of the following values. (Note that WAIT_OBJECT_0 is defined as 0 and WAIT_ABANDONED_0 is defined as 0x00000080L.)
     */
    static Int32U          JCORE_STDCALL WaitForMultipleObjectsEx(JCORE_IN Int32U eventCount, JCORE_IN WinHandle* handles, JCORE_IN bool waitAll, JCORE_IN Int32U timeout = JCORE_INFINITE, JCORE_IN bool alertable = false);
    static Int32U          JCORE_STDCALL GetLastError();


    /** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-setevent#return-value
     * \brief -
     * \param handle  A handle to the event object. The CreateEvent or OpenEvent function returns this handle.
     * \return If the function succeeds, the return value is nonzero.
     */
    static bool            JCORE_STDCALL SetEvent(JCORE_IN WinHandle handle);

    /** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-resetevent
     * \brief Sets the specified event object to the nonsignaled state.
     * \param handle A handle to the event object. The CreateEvent or OpenEvent function returns this handle.
     * \return If the function succeeds, the return value is nonzero.
     */
    static bool            JCORE_STDCALL ResetEvent(JCORE_IN WinHandle handle);

    /** https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
     * \brief Closes an open object handle.
     * \param handle A valid handle to an open object.
     * \return If the function succeeds, the return value is nonzero. If the function fails, the return value is zero. To get extended error information, call GetLastError.
     */
    static bool            JCORE_STDCALL CloseHandle(JCORE_IN WinHandle handle);


    /** https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadpriority
     * \brief Retrieves the priority value for the specified thread. This value, together with the priority class of the thread's process, determines the thread's base-priority level.
     * \param threadHandle A handle to the thread.
     * \return If the function succeeds, the return value is the thread's priority level.
     *
     * THREAD_PRIORITY_ABOVE_NORMAL      1
     * THREAD_PRIORITY_BELOW_NORMAL     -1
     * THREAD_PRIORITY_HIGHEST          2
     * THREAD_PRIORITY_IDLE             -15
     * THREAD_PRIORITY_LOWEST           -2
     * THREAD_PRIORITY_NORMAL           0
     * THREAD_PRIORITY_TIME_CRITICAL    15
     */
    static int             JCORE_STDCALL GetThreadPriority(JCORE_IN WinHandle threadHandle);

    /** https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriority
     * \brief Sets the priority value for the specified thread. This value, together with the priority class of the thread's process, determines the thread's base priority level.
     * \param threadHandle A handle to the thread whose priority value is to be set.
     * \param priority The priority value for the thread.
     * \return If the function succeeds, the return value is nonzero. If the function fails, the return value is zero. To get extended error information, call GetLastError.
     */
    static bool            JCORE_STDCALL SetThreadPriority(JCORE_IN WinHandle threadHandle, JCORE_IN Int priority);

    /** https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid
     * \brief Retrieves the thread identifier of the calling thread. 
     * \remarks Until the thread terminates, the thread identifier uniquely identifies the thread throughout the system.
     * \return The return value is the thread identifier of the calling thread.
     */
    static Int32U          JCORE_STDCALL GetCurrentThreadId();


	/**
	 * \brief https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
	 * Retrieves the fully qualified path for the file that contains the specified module. The module must have been loaded by the current process.
	 * \return the return value is the length of the string that is copied to the buffer
	 */
	static Int32U          JCORE_STDCALL GetModuleFilePath(JCORE_IN_OPT WinModule module, JCORE_OUT char* filenameBuffer, JCORE_IN int filenameBufferCapacity);
    
}; // struct WinApi




using Boundary8 = Int8;          // 8  bit 레지스터 (al)
using Boundary16 = Int16;        // 16 bit 레지스터 (ax)
using Boundary32 = Int32L;       // 32 bit 레지스터 (eax)
using Boundary64 = Int64;        // 64 bit 레지스터 (rax)

// 함수는 부분 특수화 땜에 클래스로 만듬
// 함수 인자/반환값 설명은 마이크로소프트 웹에서 복사해옴
// MSVC 어셈블리 확인해봤는데 문제 없어보인다.
// https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGe1wAyeAyYAHI%2BAEaYxBJmpAAOqAqETgwe3r56icmOAkEh4SxRMVxxdpgOqUIETMQE6T5%2BXLaY9rkM1bUE%2BWGR0bG2NXUNmc0KQ93BvUX9pQCUtqhexMjsHOYAzMHI3lgA1CYbbgDqweioAO4KAHQIh9gmGgCCj08EmCzxBu8HRwQAnvFGKxMHsACoAeSBxEM6Hur0h0NhrwAkgxkMQPowCFwIAA3TxMRz0cFQ6KwgBUeywjmY7TmBwA7FZnns2Xs8FQ9mgGONMKp4sQ9hBkgAvTCoKgQRHkhjoBmHNyHAAie0kCuZr3Z2r2mIIywYe3GRLwyAA%2BqJxoqZTC5fcICAQGj3sRaKhkABrTDoNEYrGCCCY4IuwWYAgWphWo4E750UFuowU%2B004ImgRzDOHFlPbUmRnK15a9l6g1GmqOc2WgjWsm2uEbbAOp2CaJuz3e32YtiCABskkDmGD0VD4aripjJpJZrNwb7SYbEBTdNSGbmWcL%2BcLz3en2%2BoMVAKBzDYpKRdobCNryOenf9BDM0qvcqpS7TDA12e1nO5Aj5AqFIp4OKkqPme8q/EqGyquqTKfjqbIlsQhrGhWEZRm4NrIgujrOq27pej66JdtiA5DsQI5odW0aEsS8YCMA86Nq%2B9KZhscFsnmBbPEWCFhqWKGmpRNZgfaOEtq6%2BEdkRd59qR4kUWO1GxlOM69pIjGLmGqYsWubEblxLzcduHxfES%2B5/ICwInmC8LPBOtHglSYKoAAajRcbSuYPZ7EMwBhh%2BPG6nxSFBWRCmRlRbj2XGjn2l5vn%2Beu3GbkZTzBnsLBMMEEABay7LpUwvyqhoSWGTm7K3t2OIQF5TC6exeyVdiD7OW5ymYBAdX1Vu5W8fqIUlXpyXKhwCy0JwACsvB%2BBwWikKgnBKpY1hGksKz7mYGw8KQBCaKNCweiAPZcNcPYbBoGgAJySBokgbAAHPdjJcBNE36JwkjTXt82cLwCggBoO17QscCwEgaCfHGZAUBAEPxFDIB4sg8TxGaeJcJdZpmGYZqqPdPakFgeKmpgLl4JgFxkjN200LQLr/RAETfREqbEP8nDbRDVUQgwtDs7NvBYJlRjiALhN4JilR4pg/1i/yFReO8HO8EO41i7QeARDCbMeFg30EMQeAsMrCxUAYwAKGTFNU8rMiCCIYjsFIdvyEoajfbozQGEYKDWNY%2Bia/9kALKg8TtLLAD0EJmHsEfjOgvvLZYe7jH9rQVO0LhyiMTSkIEUyFMUWRJCkAg58XOSpD0hezC0bRVBM5djOnlQCJ0dTV30JSDF0Tc9x3BddxICwKGtqzD%2B9HBTaQM1zQtHB7Hj3ksAoSN7Ojl3XNjwq4IQJAHJtXBzLwu0CxmpCHQ91wbD2l2XRoE2XRNx2lPdkhvWrn0z9989/QDQNnzGpwMwX0xa/wAVoc%2B0tiDJGcJIIAA%3D
template <typename TOperand>
struct Interlocked final
{

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedadd
    /**
     * \brief -
     * \param destination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param value The second operand.
     * \return The function returns the result of the operation.
     */
    static TOperand Add(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedcompareexchange
    /**
     * \brief -
     * \param destination A pointer to the destination value.
     * \param expected The value to compare to Destination.
     * \param desired The exchange value.
     * \return The function returns the initial value of the Destination parameter.
     */
    static TOperand CompareExchange(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand expected, TOperand desired);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchange
    /**
     * \brief -
     * \param destination A pointer to the value to be exchanged. The function sets this variable to Value, and returns its prior value.
     * \param value The value to be exchanged with the value pointed to by Target.
     * \return The function returns the initial value of the Target parameter.
     */
    static TOperand Exchange(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchangeadd
    /**
     * \brief -
     * \param destination A pointer to a variable. The value of this variable will be replaced with the result of the operation.
     * \param value The value to be added to the variable pointed to by the Addend parameter.
     * \return The function returns the initial value of the Addend parameter.
     */
    static TOperand ExchangeAdd(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedincrement
    /**
     * \brief -
     * \param destination A pointer to the variable to be incremented.
     * \return The function returns the resulting incremented value.
     */
    static TOperand Increment(JCORE_IN_OUT TOperand* destination);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockeddecrement
    /**
     * \brief -
     * \param destination A pointer to the variable to be decremented.
     * \return The function returns the resulting decremented value.
     */
    static TOperand Decrement(JCORE_IN_OUT TOperand* destination);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedand
    /**
     * \brief -
     * \param destination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param value The second operand.
     * \return The function returns the original value of the Destination parameter.
     */
    static TOperand And(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedor
    /**
     * \brief -
     * \param destination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param value The second operand.
     * \return The function returns the original value of the Destination parameter.
     */
    static TOperand Or(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value);


    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedxor
    /**
     * \brief -
     * \param destination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param value The second operand.
     * \return The function returns the original value of the Destination parameter.
     */
    static TOperand Xor(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value);
    static TOperand Read(JCORE_IN_OUT TOperand* destination) { return Interlocked::Add(destination, 0); }
}; // struct Interlocked final



template <typename TOperand>
struct Interlocked<TOperand*> final
{
    // 포인터는 x86, x64 플랫폼에 따라서 32비트, 64비트 정수형으로 각각 강제 형변환해서 사용
    // 딱히 다른 방법은 떠오르지 않는다.
    static constexpr int PlatformPtrSize = sizeof(TOperand*);

    using TOperandPtr = TOperand*;
    using TReinterpretedType = Conditional_t<PlatformPtrSize == 4, Boundary32, Boundary64>;
    using TInterlocked = Interlocked<TReinterpretedType>;

    static TOperand* Add(JCORE_IN_OUT TOperand** destination, JCORE_IN int value) {
        return reinterpret_cast<TOperand*>(TInterlocked::Add(
            reinterpret_cast<TReinterpretedType*>(destination), 
            sizeof(TOperand) * value));
    }

    static TOperand* CompareExchange(JCORE_IN_OUT TOperand** destination, JCORE_IN TOperand* expected, JCORE_IN TOperand* desired) {
        return reinterpret_cast<TOperand*>(TInterlocked::CompareExchange(
            reinterpret_cast<TReinterpretedType*>(destination),
            reinterpret_cast<TReinterpretedType>(expected),
            reinterpret_cast<TReinterpretedType>(desired)));
    }
    static TOperand* Exchange(JCORE_IN_OUT TOperand** destination, JCORE_IN TOperand* value) {
        return reinterpret_cast<TOperand*>(TInterlocked::Exchange(
            reinterpret_cast<TReinterpretedType*>(destination),
            reinterpret_cast<TReinterpretedType>(value)));
    }

    static TOperand* ExchangeAdd(JCORE_IN_OUT TOperand** destination, JCORE_IN int value) {
        return reinterpret_cast<TOperand*>(TInterlocked::ExchangeAdd(
            reinterpret_cast<TReinterpretedType*>(destination),
            sizeof(TOperand) * value));
    }

    static TOperand* Read(JCORE_IN_OUT TOperand** destination) {
        return reinterpret_cast<TOperand*>(TInterlocked::Add(reinterpret_cast<TReinterpretedType*>(destination), 0));
    }

}; // struct Interlocked final


template <>
struct Interlocked<bool> final
{
    using TInterlocked = Interlocked<Boundary8>;

    static bool CompareExchange(JCORE_IN_OUT bool* destination, JCORE_IN bool expected, JCORE_IN bool desired);
    static bool Exchange(JCORE_IN_OUT bool* destination, JCORE_IN bool value);
    static bool Read(JCORE_IN_OUT bool* destination);
}; // struct Interlocked final


extern template struct Interlocked<Int8>;
extern template struct Interlocked<Int8U>;
extern template struct Interlocked<Int16>;
extern template struct Interlocked<Int16U>;
extern template struct Interlocked<WideChar>;
extern template struct Interlocked<Int32>;
extern template struct Interlocked<Int32U>;
extern template struct Interlocked<Int32L>;
extern template struct Interlocked<Int32UL>;
extern template struct Interlocked<Int64>;
extern template struct Interlocked<Int64U>;
extern template struct Interlocked<bool>;

NS_JC_END

