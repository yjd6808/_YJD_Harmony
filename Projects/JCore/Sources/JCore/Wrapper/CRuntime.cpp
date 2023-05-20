#include <JCore/Core.h>
#include <JCore/Wrapper/CRuntime.h>

#include <cstdlib>
#include <process.h>

NS_JC_BEGIN

int
JCORE_CDECL
CRuntime::System(const char* cmd) {
    return ::system(cmd);
}

IntPtr
JCORE_CDECL
CRuntime::BeginThreadEx(CRuntime::ThreadFunc fn, void* arg, Int32U stackSize, Int32U initFlag) {
    return ::_beginthreadex(NULL, stackSize, fn, arg, initFlag, NULL);
}

void
JCORE_CDECL
CRuntime::EndThreadEx(Int32U exitCode) {
    ::_endthreadex(exitCode);
}

Int
JCORE_CDECL
CRuntime::ErrorNo() {
    Int iErrorCode = *::_errno();
    return iErrorCode;
}


IoHandle
JCORE_CDECL
CRuntime::FileOpen(const char* filename, const char* mode) {
    FILE* pFile = nullptr;

    // 반환값으로 errno_t 값을 확인할 수도 있고
    // _errno로도 확인가능하다.
    int iErrorCode = ::fopen_s(&pFile, filename, mode);

    if (iErrorCode == 0) {
        return pFile;
    }

    return nullptr;
}

int
JCORE_CDECL
CRuntime::FileWrite(const void* buffer, int size, int count, IoHandle stream) {
     return ::fwrite(buffer, size, count, (FILE*)stream);
}

int
JCORE_CDECL
CRuntime::FileRead(void* buffer, int size, int count, IoHandle stream) {
    return ::fread(buffer, size, count, (FILE*)stream);
}

int
JCORE_CDECL
CRuntime::FileReadChar(IoHandle stream) {
    return ::fgetc((FILE*)stream);
}

bool CRuntime::FileHasError(IoHandle stream) {
    // 오류 발생시 0이 아닌값, 오류가 없는 경우 0을 반환
    return ::ferror((FILE*)stream) != 0;
}

long
JCORE_CDECL
CRuntime::FileTell(IoHandle stream) {
    long iRet = ::ftell((FILE*)stream);
    return iRet == -1L ? *_errno() : iRet;
}

bool
JCORE_CDECL
CRuntime::FileSeek(IoHandle stream, long offset, int origin) {
    // 커서를 성공적으로 움직이는데 성공시 0, 실패시 0이 아닌 값을 반환한다.
    return ::fseek((FILE*)stream, offset, origin) == 0;
}


bool
JCORE_CDECL
CRuntime::FileSeekBegin(IoHandle stream, long offset) {
    return FileSeek(stream, offset, SEEK_SET);
}

bool
JCORE_CDECL
CRuntime::FileSeekCur(IoHandle stream, long offset) {
    return FileSeek(stream, offset, SEEK_CUR);
}

bool
JCORE_CDECL
CRuntime::FileSeekEnd(IoHandle stream, long offset) {
    return FileSeek(stream, offset, SEEK_END);
}

bool
JCORE_CDECL
CRuntime::FileFlush(IoHandle stream) {
    // 성공적으로 반영했다면 0
    // 실패한 경우 EOF를 반환하고 해당 파일 스트림의 error flag가 set 된다.
    // FileError로 확인가능
    return ::fflush((FILE*)stream) == 0;
}

bool
JCORE_CDECL
CRuntime::FileEOF(IoHandle stream) {
    // 끝까지 도달한 경우 0이 아닌 값, 아직 여유가 있는 경우 0
    return ::feof((FILE*)stream) != 0;
}

bool
JCORE_CDECL
CRuntime::FileClose(IoHandle stream) {
    // 성공시 0반환, 실패시 EOF 반환
    return ::fclose((FILE*)stream) == 0;
}

bool
JCORE_CDECL
CRuntime::FileDelete(const char* path) {
    // 삭제 성공시 0반환, 실패시 0이 아닌 값 반환
    return ::remove(path) == 0;
}

bool CRuntime::FileRename(const char* oldPath, const char* newPath, bool overwrite) {
    // 변경 성공시 0반환, 실패시 0이 아닌 값 반환
    if (overwrite) {
        FileDelete(newPath);
    }

    return ::rename(oldPath, newPath) == 0;
}

NS_JC_END
