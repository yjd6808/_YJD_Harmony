/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:55 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Limit.h>
#include <JCore/Wrapper/WinApi.h>

NS_JC_BEGIN

WinHandle WinApi::InvalidHandleValue = INVALID_HANDLE_VALUE;

bool
JCORE_STDCALL
WinApi::SetConsoleCursorPosition(JCORE_IN WinHandle hStdoutHandle, JCORE_IN int x, JCORE_IN int y) {
    COORD p;
    p.X = static_cast<Int16>(x);
    p.Y = static_cast<Int16>(y);
    return ::SetConsoleCursorPosition(hStdoutHandle, p) != 0;
}

bool
JCORE_STDCALL
WinApi::GetConsoleCursorPosition(JCORE_IN WinHandle hStdoutHandle, JCORE_OUT int& x, JCORE_OUT int& y) {
    CONSOLE_SCREEN_BUFFER_INFO cbsi;

    if (::GetConsoleScreenBufferInfo(hStdoutHandle, &cbsi)) {
        x = cbsi.dwCursorPosition.X;
        y = cbsi.dwCursorPosition.Y;
        return true;
    }

    return false;
}

bool
JCORE_STDCALL
WinApi::SetConsoleTextAttribute(JCORE_IN WinHandle hStdoutHandle, JCORE_IN Int16 attribute) {
    return ::SetConsoleTextAttribute(hStdoutHandle, attribute) != 0;
}


bool
JCORE_STDCALL
WinApi::SetConsoleOutputCodePage(JCORE_IN Int codePage) {
    return ::SetConsoleOutputCP(codePage) != 0;
}


Int
JCORE_STDCALL
WinApi::GetConsoleOutputCodePage() {
    return static_cast<int>(::GetConsoleOutputCP());
}

WinHandle
JCORE_STDCALL
WinApi::GetStdoutHandle() {
    return ::GetStdHandle(STD_OUTPUT_HANDLE);
}

WinHandle
JCORE_STDCALL
WinApi::GetStdinHandle() {
    return ::GetStdHandle(STD_INPUT_HANDLE);
}

WinHandle
JCORE_STDCALL
WinApi::CreateEventA(bool initialState, bool manualReset, const char* name) {
    return ::CreateEventA(NULL, manualReset ? TRUE : FALSE, initialState ? TRUE : FALSE, name);
}

Int32UL
JCORE_STDCALL
WinApi::WaitForMultipleObjectsEx(JCORE_IN Int32U eventCount, JCORE_IN WinHandle* handles, JCORE_IN bool waitAll, JCORE_IN Int32U timeout, JCORE_IN bool alertable) {
    return ::WaitForMultipleObjectsEx(eventCount, handles, waitAll ? TRUE : FALSE, timeout, alertable ? TRUE: FALSE);
}

Int32UL
JCORE_STDCALL
WinApi::GetLastError() {
    return ::GetLastError();
}

bool
JCORE_STDCALL
WinApi::SetEvent(JCORE_IN WinHandle handle) {
    return ::SetEvent(handle) != 0;
}

bool
JCORE_STDCALL
WinApi::ResetEvent(JCORE_IN WinHandle handle) {
    return ::ResetEvent(handle) != 0;   
}

bool
JCORE_STDCALL
WinApi::CloseHandle(JCORE_IN WinHandle handle) {
    return ::CloseHandle(handle) != 0;
}

int
JCORE_STDCALL
WinApi::GetThreadPriority(JCORE_IN WinHandle threadHandle) {
    return ::GetThreadPriority(threadHandle);
}

bool
JCORE_STDCALL
WinApi::SetThreadPriority(JCORE_IN WinHandle threadHandle, JCORE_IN int priority) {
    return ::SetThreadPriority(threadHandle, priority) != 0;
}


Int32U
JCORE_STDCALL
WinApi::GetCurrentThreadId() {
    return ::GetCurrentThreadId();
}

Int32U
JCORE_STDCALL
WinApi::GetModuleFilePath(WinModule module, char* filenameBuffer, int filenameBufferCapacity) {
    return GetModuleFileNameA((HMODULE)module, filenameBuffer, filenameBufferCapacity);
}


// =========================================================================================
// Interlocked Api 
// =========================================================================================


template <typename TOperand>
TOperand
Interlocked<TOperand>::Add(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value) {
    return ExchangeAdd(destination, value) + value;
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::CompareExchange(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand expected, TOperand desired) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return static_cast<TOperand>(::_InterlockedCompareExchange8(reinterpret_cast<volatile Boundary8*>(destination), desired, expected));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedCompareExchange16(reinterpret_cast<volatile Boundary16*>(destination), desired, expected));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedCompareExchange(reinterpret_cast<volatile Boundary32*>(destination), desired, expected));
    }

    return static_cast<TOperand>(::_InterlockedCompareExchange64(reinterpret_cast<volatile Boundary64*>(destination), desired, expected));
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Exchange(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return static_cast<TOperand>(::_InterlockedExchange8(reinterpret_cast<volatile Boundary8*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedExchange16(reinterpret_cast<volatile Boundary16*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedExchange(reinterpret_cast<volatile Boundary32*>(destination), value));
    }

#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedExchange64(reinterpret_cast<volatile Boundary64*>(destination), value));
#else
    return static_cast<TOperand>(::_InlineInterlockedExchange64(reinterpret_cast<volatile Boundary64*>(destination), value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::ExchangeAdd(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return static_cast<TOperand>(_InterlockedExchangeAdd8(reinterpret_cast<volatile Boundary8*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(_InterlockedExchangeAdd16(reinterpret_cast<volatile Boundary16*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedExchangeAdd(reinterpret_cast<volatile Boundary32*>(destination), value));
    }

#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedExchangeAdd64(reinterpret_cast<volatile Boundary64*>(destination), value));
#else
    return static_cast<TOperand>(::_InlineInterlockedExchangeAdd64(reinterpret_cast<volatile Boundary64*>(destination), value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Increment(JCORE_IN_OUT TOperand* destination) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return Add(destination, 1);
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedIncrement16(reinterpret_cast<volatile Boundary16*>(destination)));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedIncrement(reinterpret_cast<volatile Boundary32*>(destination)));
    }
#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedIncrement64(reinterpret_cast<volatile Boundary64*>(destination)));
#else
    return static_cast<TOperand>(::_InlineInterlockedIncrement64(reinterpret_cast<volatile Boundary64*>(destination)));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Decrement(JCORE_IN_OUT TOperand* destination) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return Add(destination, -1);
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedDecrement16(reinterpret_cast<volatile Boundary16*>(destination)));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedDecrement(reinterpret_cast<volatile Boundary32*>(destination)));
    }

#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedDecrement64(reinterpret_cast<volatile Boundary64*>(destination)));
#else
    return static_cast<TOperand>(::_InlineInterlockedDecrement64(reinterpret_cast<volatile Boundary64*>(destination)));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::And(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return static_cast<TOperand>(::_InterlockedAnd8(reinterpret_cast<volatile Boundary8*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedAnd16(reinterpret_cast<volatile Boundary16*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedAnd(reinterpret_cast<volatile Boundary32*>(destination), value));
    }

#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedAnd64(reinterpret_cast<volatile Boundary64*>(destination), value));
#else
    return static_cast<TOperand>(::_InlineInterlockedAnd64(reinterpret_cast<volatile Boundary64*>(destination), value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Or(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return static_cast<TOperand>(::_InterlockedOr8(reinterpret_cast<volatile Boundary8*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedOr16(reinterpret_cast<volatile Boundary16*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedOr(reinterpret_cast<volatile Boundary32*>(destination), value));
    }

#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedOr64(reinterpret_cast<volatile Boundary64*>(destination), value));
#else
    return static_cast<TOperand>(::_InlineInterlockedOr64(reinterpret_cast<volatile Boundary64*>(destination), value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Xor(JCORE_IN_OUT TOperand* destination, JCORE_IN TOperand value) {
    if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
        return static_cast<TOperand>(::_InterlockedXor8(reinterpret_cast<volatile Boundary8*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
        return static_cast<TOperand>(::_InterlockedXor16(reinterpret_cast<volatile Boundary16*>(destination), value));
    }

    if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
        return static_cast<TOperand>(::_InterlockedXor(reinterpret_cast<volatile Boundary32*>(destination), value));
    }

#ifdef _WIN64
    return static_cast<TOperand>(::_InterlockedXor64(reinterpret_cast<volatile Boundary64*>(destination), value));
#else
    return static_cast<TOperand>(::_InlineInterlockedXor64(reinterpret_cast<volatile Boundary64*>(destination), value));
#endif
}


bool
Interlocked<bool>::CompareExchange(JCORE_IN_OUT bool* destination, JCORE_IN bool expected, JCORE_IN bool desired) {
        Boundary8 iExpected = expected ? 1 : 0;
        Boundary8 iDesired = desired ? 1 : 0;

    return TInterlocked::CompareExchange(reinterpret_cast<Boundary8*>(destination), iExpected, iDesired) ? true : false;
}

bool
Interlocked<bool>::Exchange(bool* destination, bool value) {
    Boundary8 iValue = value ? 1 : 0;
    return TInterlocked::Exchange(reinterpret_cast<Boundary8*>(destination), iValue) ? true : false;
}

bool Interlocked<bool>::Read(bool* destination) {
    return TInterlocked::Read(reinterpret_cast<Boundary8*>(destination));
}

template struct Interlocked<Int8>;
template struct Interlocked<Int8U>;
template struct Interlocked<Int16>;
template struct Interlocked<Int16U>;
template struct Interlocked<WideChar>;
template struct Interlocked<Int32>;
template struct Interlocked<Int32U>;
template struct Interlocked<Int32L>;
template struct Interlocked<Int32UL>;
template struct Interlocked<Int64>;
template struct Interlocked<Int64U>;
template struct Interlocked<bool>;

NS_JC_END

