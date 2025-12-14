#include <JCore/Core.h>
#include <JCore/Wrapper/CRuntime.h>

#include <cstdlib>
#include <process.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
int
JCORE_CDECL
CRuntime::System(const char* _pCmd)
{
    return ::system(_pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////
IntPtr
JCORE_CDECL
CRuntime::BeginThreadEx(CRuntime::ThreadFunc _fn, void* _pArg, Int32U _stackSize, Int32U _initFlag)
{
    return ::_beginthreadex(NULL, _stackSize, _fn, _pArg, _initFlag, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
void
JCORE_CDECL
CRuntime::EndThreadEx(Int32U _exitCode)
{
    ::_endthreadex(_exitCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
Int
JCORE_CDECL
CRuntime::ErrorNo()
{
    Int errorCode = *::_errno();
    return errorCode;
}

//////////////////////////////////////////////////////////////////////////////////////////
IoHandle
JCORE_CDECL
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
int
JCORE_CDECL
CRuntime::FileWrite(const void* _pBuffer, int _size, int _count, IoHandle _pStream)
{
    return ::fwrite(_pBuffer, _size, _count, (FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
int
JCORE_CDECL
CRuntime::FileRead(void* _pBuffer, int _size, int _count, IoHandle _pStream)
{
    return ::fread(_pBuffer, _size, _count, (FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
int
JCORE_CDECL
CRuntime::FileReadChar(IoHandle _pStream)
{
    return ::fgetc((FILE*)_pStream);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CRuntime::FileHasError(IoHandle _pStream)
{
    // 오류 발생시 0이 아닌값, 오류가 없는 경우 0을 반환
    return ::ferror((FILE*)_pStream) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
long
JCORE_CDECL
CRuntime::FileTell(IoHandle _pStream)
{
    long result = ::ftell((FILE*)_pStream);
    return result == -1L ? *_errno() : result;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileSeek(IoHandle _pStream, long _offset, int _origin)
{
    // 커서를 성공적으로 움직이는데 성공시 0, 실패시 0이 아닌 값을 반환한다.
    return ::fseek((FILE*)_pStream, _offset, _origin) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileSeekBegin(IoHandle _pStream, long _offset)
{
    return FileSeek(_pStream, _offset, SEEK_SET);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileSeekCur(IoHandle _pStream, long _offset)
{
    return FileSeek(_pStream, _offset, SEEK_CUR);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileSeekEnd(IoHandle _pStream, long _offset)
{
    return FileSeek(_pStream, _offset, SEEK_END);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileFlush(IoHandle _pStream)
{
    // 성공적으로 반영했다면 0
    // 실패한 경우 EOF를 반환하고 해당 파일 스트림의 error flag가 set 된다.
    // FileError로 확인가능
    return ::fflush((FILE*)_pStream) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileEOF(IoHandle _pStream)
{
    // 끝까지 도달한 경우 0이 아닌 값, 아직 여유가 있는 경우 0
    return ::feof((FILE*)_pStream) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
CRuntime::FileClose(IoHandle _pStream)
{
    // 성공시 0반환, 실패시 EOF 반환
    return ::fclose((FILE*)_pStream) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool
JCORE_CDECL
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

NS_JC_END
