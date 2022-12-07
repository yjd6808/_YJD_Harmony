#include <JCore/Core.h>
#include <JCore/Wrapper/StdLib.h>

#include <cstdlib>

namespace JCore {

    int __cdecl System(const char* cmd) { return system(cmd); }

} // namespace JCore
