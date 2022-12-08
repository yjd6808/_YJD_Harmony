/*
 * 작성자 : 윤정도
 */

#include <JCore/Core.h>
#include <JCore/Utils/Console.h>

namespace JCore {
    namespace Detail {
        template class Console<RecursiveLock>;
        template class Console<UnusedLock>;
    } // namespace Detail
} // namespace JCore
