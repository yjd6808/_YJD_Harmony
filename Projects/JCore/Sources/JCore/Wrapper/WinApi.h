/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:46 PM
 * =====================
 * 
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>
#include <JCore/TypeTraits.h>

namespace JCore {


    bool            JCoreWinApi SetConsoleCursorPosition(In_ WinHandle hStdoutHandle, In_ int x, In_ int y);
    bool            JCoreWinApi GetConsoleCursorPosition(In_ WinHandle hStdoutHandle, Out_ int& x, Out_ int& y);
    bool            JCoreWinApi SetConsoleTextAttribute(In_ WinHandle hStdoutHandle, In_ Int16 attribute);
    bool            JCoreWinApi SetConsoleOutputCodePage(In_ Int32 codePage);
    Int             JCoreWinApi GetConsoleOutputCodePage();
    WinHandle       JCoreWinApi GetStdoutHandle();




    using Boundary8     = Int8;         // 8  bit 레지스터 (al)
    using Boundary16    = Int16;        // 16 bit 레지스터 (ax)
    using Boundary32    = Int32L;       // 32 bit 레지스터 (eax)
    using Boundary64    = Int64;        // 64 bit 레지스터 (rax)

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
        static JCoreForceInline TOperand Add(InOut_ TOperand* destination, In_ TOperand value);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedcompareexchange
        /**
         * \brief -
         * \param destination A pointer to the destination value.
         * \param expected The value to compare to Destination.
         * \param desired The exchange value.
         * \return The function returns the initial value of the Destination parameter.
         */
        static JCoreForceInline TOperand CompareExchange(InOut_ TOperand* destination, In_ TOperand expected, TOperand desired);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchange
        /**
         * \brief -
         * \param destination A pointer to the value to be exchanged. The function sets this variable to Value, and returns its prior value.
         * \param value The value to be exchanged with the value pointed to by Target.
         * \return The function returns the initial value of the Target parameter.
         */
        static JCoreForceInline TOperand Exchange(InOut_ TOperand* destination, In_ TOperand value);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedexchangeadd
        /**
         * \brief -
         * \param A pointer to a variable. The value of this variable will be replaced with the result of the operation.
         * \param The value to be added to the variable pointed to by the Addend parameter.
         * \return The function returns the initial value of the Addend parameter.
         */
        static JCoreForceInline TOperand ExchangeAdd(InOut_ TOperand* destination, In_ TOperand value);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedincrement
        /**
         * \brief -
         * \param destination A pointer to the variable to be incremented.
         * \return The function returns the resulting incremented value.
         */
        static JCoreForceInline TOperand Increment(InOut_ TOperand* destination);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockeddecrement
        /**
         * \brief -
         * \param destination A pointer to the variable to be decremented.
         * \return The function returns the resulting decremented value.
         */
        static JCoreForceInline TOperand Decrement(InOut_ TOperand* destination);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedand
        /**
         * \brief -
         * \param destination A pointer to the first operand. This value will be replaced with the result of the operation.
         * \param value The second operand.
         * \return The function returns the original value of the Destination parameter.
         */
        static JCoreForceInline TOperand And(InOut_ TOperand* destination, In_ TOperand value);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedor
        /**
         * \brief -
         * \param destination A pointer to the first operand. This value will be replaced with the result of the operation.
         * \param value The second operand.
         * \return The function returns the original value of the Destination parameter.
         */
        static JCoreForceInline TOperand Or(InOut_ TOperand* destination, In_ TOperand value);


        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedxor
        /**
         * \brief -
         * \param A pointer to the first operand. This value will be replaced with the result of the operation.
         * \param The second operand.
         * \return The function returns the original value of the Destination parameter.
         */
        static JCoreForceInline TOperand Xor(InOut_ TOperand* destination, In_ TOperand value);

    }; // struct Interlocked final



    template <typename TOperand>
    struct Interlocked<TOperand*> final
    {
        // 포인터는 x86, x64 플랫폼에 따라서 32비트, 64비트 정수형으로 각각 강제 형변환해서 사용
        // 딱히 다른 방법은 떠오르지 않는다.
        inline static constexpr int PtrSize = sizeof(TOperand);
        inline static constexpr int PlatformPtrSize = sizeof(TOperand*);

        using TOperandPtr = TOperand*;
        using TReinterpretedType = Conditional_t<PlatformPtrSize == 4, Boundary32, Boundary64>;
        using TInterlocked = Interlocked<TReinterpretedType>;

        static JCoreForceInline TOperand* Add(InOut_ TOperand** destination, In_ int value) {
            return reinterpret_cast<TOperand*>(TInterlocked::Add(
                reinterpret_cast<TReinterpretedType*>(destination), 
                PtrSize * value));
        }

        static JCoreForceInline TOperand* CompareExchange(InOut_ TOperand** destination, In_ TOperand* expected, TOperand* desired) {
            return reinterpret_cast<TOperand*>(TInterlocked::CompareExchange(
                reinterpret_cast<TReinterpretedType*>(destination),
                reinterpret_cast<TReinterpretedType>(expected),
                reinterpret_cast<TReinterpretedType>(desired)));
        }
        static JCoreForceInline TOperand* Exchange(InOut_ TOperand** destination, In_ TOperand* value) {
            return reinterpret_cast<TOperand*>(TInterlocked::Exchange(
                reinterpret_cast<TReinterpretedType*>(destination),
                reinterpret_cast<TReinterpretedType>(value)));
        }

        static JCoreForceInline TOperand* ExchangeAdd(InOut_ TOperand** destination, In_ int value) {
            return reinterpret_cast<TOperand*>(TInterlocked::Exchange(
                reinterpret_cast<TReinterpretedType*>(destination),
                PtrSize * value));
        }

    }; // struct Interlocked final

    template <>
    struct Interlocked<bool> final
    {
        using TInterlocked = Interlocked<Boundary8>;

        static JCoreForceInline bool CompareExchange(InOut_ bool* destination, In_ bool expected, bool desired) {
            Boundary8 iExpected = expected ? 1 : 0;
            Boundary8 iDesired = desired ? 1 : 0;

            return TInterlocked::CompareExchange(reinterpret_cast<Boundary8*>(destination), iExpected, iDesired) ? true : false;
        }

        static JCoreForceInline bool Exchange(InOut_ bool* destination, In_ bool value) {
            Boundary8 iValue = value ? 1 : 0;
            return TInterlocked::Exchange(reinterpret_cast<Boundary8*>(destination), iValue) ? true : false;
        } 
    }; // struct Interlocked final
} // namespace JCore