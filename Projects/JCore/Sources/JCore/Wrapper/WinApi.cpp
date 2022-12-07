/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:55 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Limit.h>
#include <JCore/Wrapper/WinApi.h>

namespace JCore {

    

    bool
    CallConvWinApi
    SetConsoleCursorPosition(In_ WinHandle hStdoutHandle, In_ int x, In_ int y) {
        COORD p;
        p.X = static_cast<Int16>(x);
        p.Y = static_cast<Int16>(y);
        return ::SetConsoleCursorPosition(hStdoutHandle, p) != 0;
    }

    bool
    CallConvWinApi
    GetConsoleCursorPosition(In_ WinHandle hStdoutHandle, Out_ int& x, Out_ int& y) {
        CONSOLE_SCREEN_BUFFER_INFO cbsi;

        if (::GetConsoleScreenBufferInfo(hStdoutHandle, &cbsi)) {
            x = cbsi.dwCursorPosition.X;
            y = cbsi.dwCursorPosition.Y;
            return true;
        }

        return false;
    }

    bool
    CallConvWinApi
    SetConsoleTextAttribute(In_ WinHandle hStdoutHandle, In_ Int16 attribute) {
        return ::SetConsoleTextAttribute(hStdoutHandle, attribute) != 0;
    }


    bool
    CallConvWinApi
    SetConsoleOutputCodePage(In_ Int codePage) {
        return ::SetConsoleOutputCP(codePage) != 0;
    }


    Int
    CallConvWinApi
    GetConsoleOutputCodePage() {
        return static_cast<int>(::GetConsoleOutputCP());
    }

    WinHandle
    CallConvWinApi
    GetStdoutHandle() {
        return ::GetStdHandle(STD_OUTPUT_HANDLE);
    }

    template <typename TOperand>
    TOperand Interlocked32Api<TOperand>::Add(InOut_ TOperand* firstOperand, In_ TOperand value){
        return static_cast<TOperand>(::InterlockedAdd(reinterpret_cast<volatile Int32L*>(firstOperand), value));
    }

    template <typename TOperand>
    TOperand Interlocked64Api<TOperand>::Add(InOut_ TOperand* firstOperand, In_ TOperand value) {
        return static_cast<TOperand>(::InterlockedAdd64(reinterpret_cast<volatile Int64*>(firstOperand), value));
    }


    template class Interlocked32Api<Int8>;
    template class Interlocked32Api<Int8U>;
    template class Interlocked32Api<Int16>;
    template class Interlocked32Api<Int16U>;
    template class Interlocked32Api<Int32>;
    template class Interlocked32Api<Int32U>;

    template class Interlocked64Api<Int64>;
    template class Interlocked64Api<Int64U>;





    /*
#define InterlockedAnd _InterlockedAnd
#define InterlockedAndAcquire _InterlockedAnd
#define InterlockedAndRelease _InterlockedAnd
#define InterlockedAndNoFence _InterlockedAnd

#define InterlockedOr _InterlockedOr
#define InterlockedOrAcquire _InterlockedOr
#define InterlockedOrRelease _InterlockedOr
#define InterlockedOrNoFence _InterlockedOr

#define InterlockedXor _InterlockedXor
#define InterlockedXorAcquire _InterlockedXor
#define InterlockedXorRelease _InterlockedXor
#define InterlockedXorNoFence _InterlockedXor

#define InterlockedIncrement _InterlockedIncrement
#define InterlockedIncrementAcquire _InterlockedIncrement
#define InterlockedIncrementRelease _InterlockedIncrement
#define InterlockedIncrementNoFence _InterlockedIncrement

#define InterlockedDecrement _InterlockedDecrement
#define InterlockedDecrementAcquire _InterlockedDecrement
#define InterlockedDecrementRelease _InterlockedDecrement
#define InterlockedDecrementNoFence _InterlockedDecrement

#define InterlockedAdd _InlineInterlockedAdd
#define InterlockedAddAcquire _InlineInterlockedAdd
#define InterlockedAddRelease _InlineInterlockedAdd
#define InterlockedAddNoFence _InlineInterlockedAdd
#define InterlockedAddNoFence64 _InlineInterlockedAdd64

#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAcquire _InterlockedExchange
#define InterlockedExchangeNoFence _InterlockedExchange

#define InterlockedExchangeAdd _InterlockedExchangeAdd
#define InterlockedExchangeAddAcquire _InterlockedExchangeAdd
#define InterlockedExchangeAddRelease _InterlockedExchangeAdd
#define InterlockedExchangeAddNoFence _InterlockedExchangeAdd

#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedCompareExchangeAcquire _InterlockedCompareExchange
#define InterlockedCompareExchangeRelease _InterlockedCompareExchange
#define InterlockedCompareExchangeNoFence _InterlockedCompareExchange

#define InterlockedExchange16 _InterlockedExchange16
*/
} // namespace JCore

