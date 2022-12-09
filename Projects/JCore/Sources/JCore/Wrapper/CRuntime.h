/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:55:29 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>
#include <JCore/Type.h>

namespace JCore {
    struct CRuntime {
        using ThreadFunc = Int32U (JCoreStdCall*)(void*);

        static int      JCoreCdecl System(In_ const char* cmd);

        /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170
         * \brief 쓰레드를 만듬
         * \param fn 실행할 함수
         * \param arg 전달할 인자
         * \param stackSize 쓰레드 스택 크기(0 = 디폴트)
         * \return If successful, each of these functions returns a handle to the newly created thread; however, if the newly created thread exits too quickly, _beginthread might not return a valid handle. (See the discussion in the Remarks section.) On an error, _beginthread returns -1L, and errno is set to EAGAIN if there are too many threads, to EINVAL if the argument is invalid or the stack size is incorrect, or to EACCES if there are insufficient resources (such as memory). On an error, _beginthreadex returns 0, and errno and _doserrno are set.
         *
         * _beginthread -> _endthread를 하면 이후에 CloseHandle을 해줄 필요가 없다.
         * https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170#examples
         * 코드보면 쓰레드 루틴 내부에서 _enthread 호출해줌
         *
         * _beginthreadex -> endthreadex를 하더라도 CloseHandle을 해줘야한다.
         *
         */
        static IntPtr   JCoreCdecl BeginThreadEx(In_ ThreadFunc fn, InOpt_ void* arg = nullptr, InOpt_ Int32U stackSize = 0, InOpt_ Int32U initFlag = 0);

        /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/endthread-endthreadex?view=msvc-170
         * \brief Terminates a thread; _endthread terminates a thread that's created by _beginthread and _endthreadex terminates a thread that's created by _beginthreadex.
         * \param exitCode Thread exit code.
         */
        static void     JCoreCdecl EndThreadEx(In_ Int32U exitCode);

        /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/get-errno?view=msvc-170
         * \brief Gets the current value of the errno global variable.
         * \return Returns zero if successful;
         */
        static Int      JCoreCdecl ErrorNo();
    };
} // namespace JCore
