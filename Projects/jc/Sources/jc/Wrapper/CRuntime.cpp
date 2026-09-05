#include "jc/Wrapper/CRuntime.h"

#include "cstdio"
#include "cstdlib"
#include "process.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
int
JC_CDECL
CRuntime::System(const char* _pCmd)
{
    return ::system(_pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
_ptr
JC_CDECL
CRuntime::BeginThreadEx(CRuntime::ThreadFunc _fn, void* _pArg, _u32 _stackSize, _u32 _initFlag)
{
    return ::_beginthreadex(NULL, _stackSize, _fn, _pArg, _initFlag, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
void
JC_CDECL
CRuntime::EndThreadEx(_u32 _exitCode)
{
    ::_endthreadex(_exitCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32
JC_CDECL
CRuntime::ErrorNo()
{
    _s32 errorCode = *::_errno();
    return errorCode;
}

//////////////////////////////////////////////////////////////////////////////////////////
_iohandle
JC_CDECL
CRuntime::FileOpen(const char* _pFilename, const char* _pMode)
{
    FILE* pFile = nullptr;

    // 반환값으로 errno_t 값을 확인할 수도 있고
    // _errno로도 확인가능하다.
    int errorCode = ::fopen_s(&pFile, _pFilename, _pMode);

    if (errorCode == 0)
    {
        return pFile;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
size_t
JC_CDECL
CRuntime::FileWrite(const void* _pBuffer, size_t _size, size_t _count, _iohandle _pStream)
{
    return ::fwrite(_pBuffer, _size, _count, (FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
size_t
JC_CDECL
CRuntime::FileRead(void* _pBuffer, size_t _size, size_t _count, _iohandle _pStream)
{
    return ::fread(_pBuffer, _size, _count, (FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
int
JC_CDECL
CRuntime::FileReadChar(_iohandle _pStream)
{
    return ::fgetc((FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CRuntime::FileHasError(_iohandle _pStream)
{
    // 오류 발생시 0이 아닌값, 오류가 없는 경우 0을 반환
    return ::ferror((FILE*)_pStream) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
long
JC_CDECL
CRuntime::FileTell(_iohandle _pStream)
{
    long result = ::ftell((FILE*)_pStream);
    return result == -1L ? *_errno() : result;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeek(_iohandle _pStream, long _offset, int _origin)
{
    // 커서를 성공적으로 움직이는데 성공시 0, 실패시 0이 아닌 값을 반환한다.
    return ::fseek((FILE*)_pStream, _offset, _origin) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeekBegin(_iohandle _pStream, long _offset)
{
    return FileSeek(_pStream, _offset, SEEK_SET);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeekCur(_iohandle _pStream, long _offset)
{
    return FileSeek(_pStream, _offset, SEEK_CUR);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeekEnd(_iohandle _pStream, long _offset)
{
    return FileSeek(_pStream, _offset, SEEK_END);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64
JC_CDECL
CRuntime::FileTell64(_iohandle _pStream)
{
    // 실패시 -1 반환 — 원인은 CRuntime::ErrorNo()로 조회 (위치값과 구별 불가한 errno 매핑 방지)
    return (_s64)::_ftelli64((FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeek64(_iohandle _pStream, _s64 _offset, int _origin)
{
    return ::_fseeki64((FILE*)_pStream, (__int64)_offset, _origin) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeekBegin64(_iohandle _pStream, _s64 _offset)
{
    return FileSeek64(_pStream, _offset, SEEK_SET);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeekCur64(_iohandle _pStream, _s64 _offset)
{
    return FileSeek64(_pStream, _offset, SEEK_CUR);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileSeekEnd64(_iohandle _pStream, _s64 _offset)
{
    return FileSeek64(_pStream, _offset, SEEK_END);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileFlush(_iohandle _pStream)
{
    // 성공적으로 반영했다면 0
    // 실패한 경우 EOF를 반환하고 해당 파일 스트림의 error flag가 set 된다.
    // FileError로 확인가능
    return ::fflush((FILE*)_pStream) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileEOF(_iohandle _pStream)
{
    // 끝까지 도달한 경우 0이 아닌 값, 아직 여유가 있는 경우 0
    return ::feof((FILE*)_pStream) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileClose(_iohandle _pStream)
{
    // 성공시 0반환, 실패시 EOF 반환
    return ::fclose((FILE*)_pStream) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JC_CDECL
CRuntime::FileDelete(const char* _pPath)
{
    // 삭제 성공시 0반환, 실패시 0이 아닌 값 반환
    return ::remove(_pPath) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CRuntime::FileRename(const char* _pOldPath, const char* _pNewPath, bool _overwrite)
{
    // 변경 성공시 0반환, 실패시 0이 아닌 값 반환
    if (_overwrite)
    {
        FileDelete(_pNewPath);
    }

    return ::rename(_pOldPath, _pNewPath) == 0;
}

NS_END
