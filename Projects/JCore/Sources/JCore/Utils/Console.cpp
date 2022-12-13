/*
 * 작성자 : 윤정도
 */

#include <JCore/Core.h>
#include <JCore/Utils/Console.h>

namespace JCore {
    namespace Detail {
    } // namespace Detail

    template class Detail::Console<RecursiveLock>;
    template class Detail::Console<UnusedLock>;
} // namespace JCore
