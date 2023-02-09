#pragma once
#define _CRTDBG_MAP_ALLOC


#include <JCore/Random.h>
#include <JCore/TypeTraits.h>
#include <JCore/Time.h>
#include <JCore/Limit.h>
#include <JCore/ByteUnit.h>
#include <JCore/Env.h>

#include <JCore/Allocator/DefaultAllocator.h>
#include <JCore/Allocator/DefaultArrayAllocator.h>

#include <JCore/Container/HashMap.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Vector.h>

#include <JCore/Primitives/Atomic.h>
#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Primitives/String.h>

#include <JCore/FileSystem/Path.h>
#include <JCore/Encoding/CodePage.h>

#include <JCore/Pool/MemoryPoolManager.h>


#include <JCore/Sync/SpinLock.h>
#include <JCore/Sync/WaitHandle.h>

#include <JCore/Wrapper/WinApi.h>
#include <JCore/Wrapper/CRuntime.h>

#include <JCore/Utils/Console.h>

#include <JCore/Threading/Thread.h>
#include <JCore/Threading/ThreadLocal.h>

USING_NS_JC;
USING_NS_STD;

using Console = SafeConsole;