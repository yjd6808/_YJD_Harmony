/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:55:29 PM
 * =====================
 *
 */


#pragma once

#include "jc/Define.h"
#include "jc/Type.h"

NS_JC_BEGIN
struct CRuntime {
    using ThreadFunc = _u32 (JC_STDCALL*)(void*);

    static int      JC_CDECL System( const char* _pCmd);

    /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170
     * \brief 쓰레드를 만듬
     * \param _fn 실행할 함수
     * \param _pArg 전달할 인자
     * \param _stackSize 쓰레드 스택 크기(0 = 디폴트)
     * \param _initFlag
     * \return If successful, each of these functions returns a handle to the newly created thread; however, if the newly created thread exits too quickly, _beginthread might not return a valid handle. (See the discussion in the Remarks section.) On an error, _beginthread returns -1L, and errno is set to EAGAIN if there are too many threads, to EINVAL if the argument is invalid or the stack size is incorrect, or to EACCES if there are insufficient resources (such as memory). On an error, _beginthreadex returns 0, and errno and _doserrno are set.
     *
     * _beginthread -> _endthread를 하면 이후에 CloseHandle을 해줄 필요가 없다.
     * https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170#examples
     * 코드보면 쓰레드 루틴 내부에서 _enthread 호출해줌
     *
     * _beginthreadex -> endthreadex를 하더라도 CloseHandle을 해줘야한다.
     *
     */
    static _ptr   JC_CDECL BeginThreadEx( ThreadFunc _fn, IN_OPT void* _pArg = nullptr, IN_OPT _u32 _stackSize = 0, IN_OPT _u32 _initFlag = 0);

    /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/endthread-endthreadex?view=msvc-170
     * \brief Terminates a thread; _endthread terminates a thread that's created by _beginthread and _endthreadex terminates a thread that's created by _beginthreadex.
     * \param _exitCode Thread exit code.
     */
    static void     JC_CDECL EndThreadEx( _u32 _exitCode);

    /** https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/get-errno?view=msvc-170
     * 참고: C 라이브러리에서 전역 에러값을 체크하는 함수들은 함수실행전 0로 초기화시킴 (https://cplusplus.com/reference/cerrno/errno/)
     * \brief Gets the current value of the errno global variable.
     * \return Returns zero if successful;
     */
    static _s32      JC_CDECL ErrorNo();


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
	static _iohandle JC_CDECL FileOpen(const char* _pFilename, const char* _pMode);


	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fwrite
	 * count를 0으로 전달한 경우 아무 행동도 하지 않는다.
	 *
	 * \param _pBuffer 버퍼의 크기
	 * \param _size 원소 크기
	 * \param _count 원소 갯수
	 * \param _pStream 입출력 핸들
	 * \return 쓴 원소 갯수 (count보다 작은 경우 오류가 발생한 경우이다.)
	 * 
	 */
	static size_t JC_CDECL FileWrite(const void* _pBuffer, size_t _size, size_t _count, _iohandle _pStream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fread
	 * 스트림에서 size만큼 count번 읽어서 buffer에 저장한다.
	 * size또는 count가 0일 경우 아무것도 안한다.
	 * stream에서 size * count 만큼 읽어서 buffer에 저장한다.
	 * \return 읽은 원소 갯수
	 */
	static size_t JC_CDECL FileRead(void* _pBuffer, size_t _size, size_t _count, _iohandle _pStream);


	/**
	 * \brief https://en.cppreference.com/w/c/io/fgetc
	 * stream에서 1개의 문자를 읽어서 int 형으로 변환해서 반환한다.
	 * \return 성공시 문자 값을, 실패시 EOF를 반환한다.
	 */
	static int JC_CDECL FileReadChar(_iohandle _pStream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/ferror
	 * 파일 스트림에 오류가 발생했는지 체크용
	 */
	static bool JC_CDECL FileHasError(_iohandle _pStream);


	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/ftell
	 * 현재 파일이 바이너리 모드로 열린경우에만 사용 가능
	 * \return ErrorCode 값 (성공시 현재 스트림의 커서 위치, 실패시 errno 값을 반환한다.)
	 */
	static long JC_CDECL FileTell(_iohandle _pStream);


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
	static bool JC_CDECL FileSeek(_iohandle _pStream, long _offset, int _origin);
	static bool JC_CDECL FileSeekBegin(_iohandle _pStream, long _offset);
	static bool JC_CDECL FileSeekCur(_iohandle _pStream, long _offset);
	static bool JC_CDECL FileSeekEnd(_iohandle _pStream, long _offset);

	/**
	 * 64비트 파일 오프셋 계열 (_fseeki64/_ftelli64 기반, 2GB 이상 파일 지원)
	 * \return FileTell64: 성공시 커서 위치, 실패시 -1 (원인은 ErrorNo()로 조회). FileSeek64*: 성공 여부
	 */
	static _s64 JC_CDECL FileTell64(_iohandle _pStream);
	static bool JC_CDECL FileSeek64(_iohandle _pStream, _s64 _offset, int _origin);
	static bool JC_CDECL FileSeekBegin64(_iohandle _pStream, _s64 _offset);
	static bool JC_CDECL FileSeekCur64(_iohandle _pStream, _s64 _offset);
	static bool JC_CDECL FileSeekEnd64(_iohandle _pStream, _s64 _offset);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fflush
	 *  - 파일 스트림 버퍼에 내용을 파일에 반영한다.
	 *  - nullptr로 전달시 모든 열린 스트림에 대해서 flush가 수행된다.
	 * \return 오류 발생시 false 반환
	 */
	static bool JC_CDECL FileFlush(_iohandle _pStream);

	/**
	 * \brief https://en.cppreference.com/w/c/io/feof
	 * 파일 스트림이 끝까지 도달했는지 체크용
	 */
	static bool JC_CDECL FileEOF(_iohandle _pStream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/fclose
	 * stream을 닫는다.
	 */
	static bool JC_CDECL FileClose(_iohandle _pStream);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/remove
	 * 파일을 삭제한다.
	 */
	static bool JC_CDECL FileDelete(const char* _pPath);

	/**
	 * \brief https://en.cppreference.com/w/cpp/io/c/rename
	 * 파일의 이름을 변경한다.
	 */
	static bool JC_CDECL FileRename(const char* _pOldPath, const char* _pNewPath, bool _overwrite = true);
};
NS_END
