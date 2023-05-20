/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:55:29 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>
#include <JCore/Type.h>

NS_JC_BEGIN
struct CRuntime {
    using ThreadFunc = Int32U (JCORE_STDCALL*)(void*);

    static int      JCORE_CDECL System(JCORE_IN const char* cmd);

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
    static IntPtr   JCORE_CDECL BeginThreadEx(JCORE_IN ThreadFunc fn, JCORE_IN_OPT void* arg = nullptr, JCORE_IN_OPT Int32U stackSize = 0, JCORE_IN_OPT Int32U initFlag = 0);

    /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/endthread-endthreadex?view=msvc-170
     * \brief Terminates a thread; _endthread terminates a thread that's created by _beginthread and _endthreadex terminates a thread that's created by _beginthreadex.
     * \param exitCode Thread exit code.
     */
    static void     JCORE_CDECL EndThreadEx(JCORE_IN Int32U exitCode);

    /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/get-errno?view=msvc-170
     * \brief Gets the current value of the errno global variable.
     * \return Returns zero if successful;
     */
    static Int      JCORE_CDECL ErrorNo();


	/**
	 * \brief https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=msvc-170
	 * 파일를 특정 mode로 열기
	 *
	 * r: 파일이 무조건 존재해야하고, 읽기 전용으로 연다.
	 * w: 빈 파일을 새로 만든다, 기존에 존재하면 덮어쒸운다. 쓰기 전용으로 연다.
	 * a: 파일을 열고 이어서 작성할 수 있도록 마지막 위치에 커서를 둔다. 파일이 없을 경우 생성한다.
	 * r+: r 기능을 포함하고 쓰기도 가능하도록 한다.
	 * w+: w 기능을 포함하고 읽기도 가능하도록 한다.
	 * a+: a 기능을 포함하고 읽기도 가능하도록 한다.
	 *
	 * ccs 플래그를 줘서 인코딩 형식을 지정할 수도 있다.
	 * "w+, ccs=UTF-8" 뭐 이런식으로
	 */
	static IoHandle JCORE_CDECL FileOpen(const char* filename, const char* mode);


	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fwrite
	 * count를 0으로 전달한 경우 아무 행동도 하지 않는다.
	 *
	 * \param buffer 버퍼의 크기
	 * \param size 원소 크기
	 * \param count 원소 갯수
	 * \param stream 입출력 핸들
	 * \return 쓴 원소 갯수 (count보다 작은 경우 오류가 발생한 경우이다.)
	 * 
	 */
	static int JCORE_CDECL FileWrite(const void* buffer, int size, int count, IoHandle stream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fread
	 * 스트림에서 size만큼 count번 읽어서 buffer에 저장한다.
	 * size또는 count가 0일 경우 아무것도 안한다.
	 * stream에서 size * count 만큼 읽어서 buffer에 저장한다.
	 * \return 읽은 원소 갯수
	 */
	static int JCORE_CDECL FileRead(void* buffer, int size, int count, IoHandle stream);


	/**
	 * \brief https://en.cppreference.com/w/c/io/fgetc
	 * stream에서 1개의 문자를 읽어서 int 형으로 변환해서 반환한다.
	 * \return 성공시 문자 값을, 실패시 EOF를 반환한다.
	 */
	static int JCORE_CDECL FileReadChar(IoHandle stream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/ferror
	 * 파일 스트림에 오류가 발생했는지 체크용
	 */
	static bool JCORE_CDECL FileHasError(IoHandle stream);


	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/ftell
	 * 현재 파일이 바이너리 모드로 열린경우에만 사용 가능
	 * \return ErrorCode 값 (성공시 현재 스트림의 커서 위치, 실패시 errno 값을 반환한다.)
	 */
	static long JCORE_CDECL FileTell(IoHandle stream);


	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fseek
	 * origin 기준으로부터 상대적인 offset 만큼 커서를 이동한다.
	 * origin 값은 다음과 같다.
	 *
	 * SEEK_SET
	 * SEEK_CUR
	 * SEEK_END
	 * binary 스트림인 경우 SEEK_END는 사용불가능하다.
	 *
	 * \return 성공적으로 움직였는지 여부
	 */
	static bool JCORE_CDECL FileSeek(IoHandle stream, long offset, int origin);
	static bool JCORE_CDECL FileSeekBegin(IoHandle stream, long offset);
	static bool JCORE_CDECL FileSeekCur(IoHandle stream, long offset);
	static bool JCORE_CDECL FileSeekEnd(IoHandle stream, long offset);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fflush
	 *  - 파일 스트림 버퍼에 내용을 파일에 반영한다.
	 *  - nullptr로 전달시 모든 열린 스트림에 대해서 flush가 수행된다.
	 * \return 오류 발생시 false 반환
	 */
	static bool JCORE_CDECL FileFlush(IoHandle stream);

	/**
	 * \brief https://en.cppreference.com/w/c/io/feof
	 * 파일 스트림이 끝까지 도달했는지 체크용
	 */
	static bool JCORE_CDECL FileEOF(IoHandle stream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fclose
	 * stream을 닫는다.
	 */
	static bool JCORE_CDECL FileClose(IoHandle stream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/remove
	 * 파일을 삭제한다.
	 */
	static bool JCORE_CDECL FileDelete(const char* path);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/rename
	 * 파일의 이름을 변경한다.
	 */
	static bool JCORE_CDECL FileRename(const char* oldPath, const char* newPath, bool overwrite = true);
};
NS_JC_END
