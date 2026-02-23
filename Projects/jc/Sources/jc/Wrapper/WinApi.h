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

#include <jc/Type.h>
#include <jc/Define.h>
#include <jc/TypeTraits.h>


NS_JC_BEGIN

struct WinApi {
	#pragma region Win32 Struct
    struct ListEntry {
        ListEntry* Flink;
        ListEntry* Blink;
    };

    struct CriticalSection;
    struct CriticalSectionDebug {
        _s16       Type;
        _s16       CreatorBackTraceIndex;
        CriticalSection* CriticalSection;
        ListEntry   ProcessLocksList;
        _u32l     EntryCount;
        _u32l     ContentionCount;
        _u32l     Flags;
        _s16       CreatorBackTraceIndexHigh;
        _s16       SpareWORD;
    };
	#pragma pack(push, 8)
    struct CriticalSection {
        CriticalSectionDebug DebugInfo;

        //
        //  The following three fields control entering and exiting the critical
        //  section for the resource
        //

        _s32l LockCount;
        _s32l RecursionCount;
        _whandle OwningThread;        // from the thread's ClientId->UniqueThread
        _whandle LockSemaphore;
        _ptr SpinCount;            // force size on 64-bit systems when packed
    };
	#pragma pack(pop)

	#pragma endregion

    static _whandle InvalidHandleValue;

    static bool            JC_CDECL SetConsoleCursorPosition( _whandle _stdoutHandle,  int _x,  int _y);
	static bool            JC_CDECL GetConsoleCursorPosition( _whandle _stdoutHandle, OUT int& _x, OUT int& _y);
	static bool            JC_CDECL SetConsoleTextAttribute( _whandle _stdoutHandle,  _s16 _attribute);
	static bool            JC_CDECL SetConsoleOutputCodePage( _s32 _codePage);
	static _s32             JC_CDECL GetConsoleOutputCodePage();


	/** https://learn.microsoft.com/en-us/windows/console/getstdhandle
	 * \brief Retrieves a handle to the specified standard device
	 * \return The standard output device. Initially, this is the active console screen buffer,
	 */
	static _whandle       JC_CDECL GetStdoutHandle();
	static _whandle       JC_CDECL GetStdinHandle();



	/** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventa
	 * \brief Creates or opens a named or unnamed event object.
	 * \param _initialState If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.
	 * \param _manualReset If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled. If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released.
	 * \param _pName The name of the event object. The name is limited to MAX_PATH characters. Name comparison is case sensitive.
	 * \return If the function succeeds, the return value is a handle to the event object. If the named event object existed before the function call, the function returns a handle to the existing object and GetLastError returns ERROR_ALREADY_EXISTS.
	 *         If the function fails, the return value is NULL. To get extended error information, call GetLastError.
	 */
	static _whandle       JC_CDECL CreateEventA(bool _initialState,  bool _manualReset, IN_OPT const char* _pName = nullptr);

	/** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitformultipleobjectsex
	 * \brief Waits until one or all of the specified objects are in the signaled state, an I/O completion routine or asynchronous procedure call (APC) is queued to the thread, or the time-out interval elapses.
	 * \param _eventCount The number of object handles to wait for in the array pointed to by lpHandles. The maximum number of object handles is MAXIMUM_WAIT_OBJECTS. This parameter cannot be zero.
	 * \param _pHandles An array of object handles. For a list of the object types whose handles can be specified, see the following Remarks section. The array can contain handles of objects of different types. It may not contain multiple copies of the same handle.
	 * \param _waitAll If this parameter is TRUE, the function returns when the state of all objects in the lpHandles array is set to signaled. If FALSE, the function returns when the state of any one of the objects is set to signaled. In the latter case, the return value indicates the object whose state caused the function to return.
	 * \param _timeout The time-out interval, in milliseconds. If a nonzero value is specified, the function waits until the specified objects are signaled, an I/O completion routine or APC is queued, or the interval elapses. If dwMilliseconds is zero, the function does not enter a wait state if the criteria is not met; it always returns immediately. If dwMilliseconds is INFINITE, the function will return only when the specified objects are signaled or an I/O completion routine or APC is queued.
	 * \param _alertable If this parameter is TRUE and the thread is in the waiting state, the function returns when the system queues an I/O completion routine or APC, and the thread runs the routine or function. Otherwise, the function does not return and the completion routine or APC function is not executed.
	 * \return If the function succeeds, the return value indicates the event that caused the function to return. It can be one of the following values. (Note that WAIT_OBJECT_0 is defined as 0 and WAIT_ABANDONED_0 is defined as 0x00000080L.)
	 */
	static _u32l          JC_CDECL WaitForMultipleObjectsEx(_u32 _eventCount, _whandle* _pHandles, bool _waitAll, _u32 _timeout = JC_INFINITE, bool _alertable = false);
	static _u32l          JC_CDECL WaitForSingleObject(_whandle _handle, _u32 _timeout = JC_INFINITE);
	static _u32l          JC_CDECL GetLastError();


	/** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-setevent#return-value
	 * \brief -
	 * \param _handle  A handle to the event object. The CreateEvent or OpenEvent function returns this handle.
	 * \return If the function succeeds, the return value is nonzero.
	 */
	static bool            JC_CDECL SetEvent(_whandle _handle);

	/** https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-resetevent
	 * \brief Sets the specified event object to the nonsignaled state.
	 * \param _handle A handle to the event object. The CreateEvent or OpenEvent function returns this handle.
	 * \return If the function succeeds, the return value is nonzero.
	 */
	static bool            JC_CDECL ResetEvent(_whandle _handle);

	/** https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
	 * \brief Closes an open object handle.
	 * \param _handle A valid handle to an open object.
	 * \return If the function succeeds, the return value is nonzero. If the function fails, the return value is zero. To get extended error information, call GetLastError.
	 */
	static bool            JC_CDECL CloseHandle( _whandle _handle);


	/** https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadpriority
	 * \brief Retrieves the priority value for the specified thread. This value, together with the priority class of the thread's process, determines the thread's base-priority level.
	 * \param _threadHandle A handle to the thread.
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
	static int             JC_CDECL GetThreadPriority(_whandle _threadHandle);

	/** https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriority
	 * \brief Sets the priority value for the specified thread. This value, together with the priority class of the thread's process, determines the thread's base priority level.
	 * \param _threadHandle A handle to the thread whose priority value is to be set.
	 * \param _priority The priority value for the thread.
	 * \return If the function succeeds, the return value is nonzero. If the function fails, the return value is zero. To get extended error information, call GetLastError.
	 */
	static bool            JC_CDECL SetThreadPriority( _whandle _threadHandle,  _s32 _priority);

	/** https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid
	 * \brief Retrieves the thread identifier of the calling thread.
	 * \remarks Until the thread terminates, the thread identifier uniquely identifies the thread throughout the system.
	 * \return The return value is the thread identifier of the calling thread.
	 */
	static _u32          JC_CDECL GetCurrentThreadId();


	/**
	 * \brief https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
	 * Retrieves the fully qualified path for the file that contains the specified module. The module must have been loaded by the current process.
	 * \return the return value is the length of the string that is copied to the buffer
	 */
	static _u32          JC_CDECL GetModuleFilePath(IN_OPT _wmodule _module, OUT char* _pFilenameBuffer,  int _filenameBufferCapacity);
    
}; // struct WinApi




using Boundary8 = _s8;          // 8  bit 레지스터 (al)
using Boundary16 = _s16;        // 16 bit 레지스터 (ax)
using Boundary32 = _s32l;       // 32 bit 레지스터 (eax)
using Boundary64 = _s64;        // 64 bit 레지스터 (rax)

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
     * \param _pDestination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param _value The second operand.
     * \return The function returns the result of the operation.
     */
	static TOperand Add(IN_OUT TOperand* _pDestination, TOperand _value);

	// https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedcompareexchange
	/**
	 * \brief -
	 * \param _pDestination A pointer to the destination value.
	 * \param _expected The value to compare to Destination.
	 * \param _desired The exchange value.
	 * \return The function returns the initial value of the Destination parameter.
	 */
	static TOperand CompareExchange(IN_OUT TOperand* _pDestination, TOperand _expected, TOperand _desired);

	// https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchange
	/**
	 * \brief -
	 * \param _pDestination A pointer to the value to be exchanged. The function sets this variable to Value, and returns its prior value.
	 * \param _value The value to be exchanged with the value pointed to by Target.
	 * \return The function returns the initial value of the Target parameter.
	 */
	static TOperand Exchange(IN_OUT TOperand* _pDestination, TOperand _value);

	// https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchangeadd
	/**
	 * \brief -
	 * \param _pDestination A pointer to a variable. The value of this variable will be replaced with the result of the operation.
	 * \param _value The value to be added to the variable pointed to by the Addend parameter.
     * \return The function returns the initial value of the Addend parameter.
     */
    static TOperand ExchangeAdd(IN_OUT TOperand* _pDestination,  TOperand _value);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedincrement
    /**
     * \brief -
     * \param _pDestination A pointer to the variable to be incremented.
     * \return The function returns the resulting incremented value.
     */
    static TOperand Increment(IN_OUT TOperand* _pDestination);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockeddecrement
    /**
     * \brief -
     * \param _pDestination A pointer to the variable to be decremented.
     * \return The function returns the resulting decremented value.
     */
    static TOperand Decrement(IN_OUT TOperand* _pDestination);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedand
    /**
     * \brief -
     * \param _pDestination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param _value The second operand.
     * \return The function returns the original value of the Destination parameter.
     */
    static TOperand And(IN_OUT TOperand* _pDestination,  TOperand _value);

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedor
    /**
     * \brief -
     * \param _pDestination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param _value The second operand.
     * \return The function returns the original value of the Destination parameter.
     */
    static TOperand Or(IN_OUT TOperand* _pDestination,  TOperand _value);


    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedxor
    /**
     * \brief -
     * \param _pDestination A pointer to the first operand. This value will be replaced with the result of the operation.
     * \param _value The second operand.
     * \return The function returns the original value of the Destination parameter.
     */
    static TOperand Xor(IN_OUT TOperand* _pDestination,  TOperand _value);
    static TOperand Read(IN_OUT TOperand* _pDestination) { return Interlocked::Add(_pDestination, 0); }
}; // struct Interlocked final



template <typename TOperand>
struct Interlocked<TOperand*> final
{
    // 포인터는 x86, x64 플랫폼에 따라서 32비트, 64비트 정수형으로 각각 강제 형변환해서 사용
    // 딱히 다른 방법은 떠오르지 않는다.
    static constexpr int PLATFORM_PTR_SIZE = sizeof(TOperand*);

    using TOperandPtr = TOperand*;
    using TReinterpretedType = Conditional_t<PLATFORM_PTR_SIZE == 4, Boundary32, Boundary64>;
    using TInterlocked = Interlocked<TReinterpretedType>;

    static TOperand* Add(IN_OUT TOperand** _ppDestination,  int _value) 
	{
        return reinterpret_cast<TOperand*>(TInterlocked::Add(
            reinterpret_cast<TReinterpretedType*>(_ppDestination), 
            sizeof(TOperand) * _value));
    }

    static TOperand* CompareExchange(IN_OUT TOperand** _ppDestination,  TOperand* _pExpected,  TOperand* _pDesired) 
	{
        return reinterpret_cast<TOperand*>(TInterlocked::CompareExchange(
            reinterpret_cast<TReinterpretedType*>(_ppDestination),
            reinterpret_cast<TReinterpretedType>(_pExpected),
            reinterpret_cast<TReinterpretedType>(_pDesired)));
    }
    static TOperand* Exchange(IN_OUT TOperand** _ppDestination,  TOperand* _pValue) 
	{
        return reinterpret_cast<TOperand*>(TInterlocked::Exchange(
            reinterpret_cast<TReinterpretedType*>(_ppDestination),
            reinterpret_cast<TReinterpretedType>(_pValue)));
    }

    static TOperand* ExchangeAdd(IN_OUT TOperand** _ppDestination,  int _value) 
	{
        return reinterpret_cast<TOperand*>(TInterlocked::ExchangeAdd(
            reinterpret_cast<TReinterpretedType*>(_ppDestination),
            sizeof(TOperand) * _value));
    }

    static TOperand* Read(IN_OUT TOperand** _ppDestination) 
	{
        return reinterpret_cast<TOperand*>(TInterlocked::Add(reinterpret_cast<TReinterpretedType*>(_ppDestination), 0));
    }

}; // struct Interlocked final


template <>
struct Interlocked<bool> final
{
    using TInterlocked = Interlocked<Boundary8>;

    static bool CompareExchange(IN_OUT bool* _pDestination,  bool _expected,  bool _desired);
    static bool Exchange(IN_OUT bool* _pDestination,  bool _value);
    static bool Read(IN_OUT bool* _pDestination);
}; // struct Interlocked final


extern template struct Interlocked<_s8>;
extern template struct Interlocked<_u8>;
extern template struct Interlocked<_s16>;
extern template struct Interlocked<_u16>;
extern template struct Interlocked<_s16c>;
extern template struct Interlocked<_s32>;
extern template struct Interlocked<_u32>;
extern template struct Interlocked<_s32l>;
extern template struct Interlocked<_u32l>;
extern template struct Interlocked<_s64>;
extern template struct Interlocked<_u64>;
extern template struct Interlocked<bool>;

NS_END

