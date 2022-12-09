#include <JCore/Core.h>
#include <JCore/Wrapper/CRuntime.h>

#include <cstdlib>
#include <process.h>

namespace JCore {

    int
    JCoreCdecl
    CRuntime::System(const char* cmd) {
        return system(cmd);
    }

    IntPtr
    JCoreCdecl
    CRuntime::BeginThreadEx(CRuntime::ThreadFunc fn, void* arg, Int32U stackSize, Int32U initFlag) {
        return ::_beginthreadex(NULL, stackSize, fn, arg, initFlag, NULL);
    }

    void
    JCoreCdecl
    CRuntime::EndThreadEx(Int32U exitCode) {
        ::_endthreadex(exitCode);
    }

    Int
    JCoreCdecl
    CRuntime::ErrorNo() {
        Int iErrorCode = *::_errno();
        return iErrorCode;
    }
} // namespace JCore
