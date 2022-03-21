#include <JCore/Core.h>
#include <JCore/LockGuard.h>

using namespace JCore;

template class LockGuard<CriticalSectionMutex>;
template class LockGuard<EventMutex>;

template class ReaderWriterLockGuard<ReaderWriterLockSlim, ReaderWriterLockMode::Read>;
template class ReaderWriterLockGuard<ReaderWriterLockSlim, ReaderWriterLockMode::Write>;