/*
 * 작성자 : 윤정도
 */

#include <JCore/Core.h>
#include <JCore/Utils/Console.h>

NS_JC_BEGIN
    NS_DETAIL_BEGIN
    NS_DETAIL_END

    template class Detail::Console<RecursiveLock>;
    template class Detail::Console<UnusedLock>;
NS_JC_END
