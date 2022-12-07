/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:46 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>

namespace JCore {

    bool            CallConvWinApi SetConsoleCursorPosition(In_ WinHandle hStdoutHandle, In_ int x, In_ int y);
    bool            CallConvWinApi GetConsoleCursorPosition(In_ WinHandle hStdoutHandle, Out_ int& x, Out_ int& y);
    bool            CallConvWinApi SetConsoleTextAttribute(In_ WinHandle hStdoutHandle, In_ Int16 attribute);
    bool            CallConvWinApi SetConsoleOutputCodePage(In_ Int32 codePage);
    Int             CallConvWinApi GetConsoleOutputCodePage();
    WinHandle       CallConvWinApi GetStdoutHandle();

    // 함수로는 부분 특수화땜에 코드가 복잡해져서 클래스로 만듬
    template <typename TOperand>
    struct Interlocked32Api
    {
        // 첫번째 인자: In: 대상 Out: 결과값이 저장됨
        // 두번째 인자: In: 더해질 값
        // 반환값: 수행한 후의 결과값
        static TOperand Add(InOut_ TOperand* firstOperand, In_ TOperand value);
    };

    template <typename TOperand>
    struct Interlocked64Api
    {
        static TOperand Add(InOut_ TOperand* firstOperand, In_ TOperand value);
    };

    
} // namespace JCore