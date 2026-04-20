#pragma once

#include "jc/PCH/Core.h"

#include "jc/Random.h"
#include "jc/TypeTraits.h"
#include "jc/Time.h"
#include "jc/Ascii.h"
#include "jc/Limit.h"
#include "jc/ByteUnit.h"
#include "jc/Env.h"

#include "jc/Utils/Concatenation.h"

#include "jc/Container/HashMap.h"
#include "jc/Container/HashSet.h"
#include "jc/Container/TreeMap.h"
#include "jc/Container/TreeSet.h"
#include "jc/Container/ListQueue.h"
#include "jc/Container/ArrayQueue.h"
#include "jc/Container/Vector.h"
#include "jc/Container/Properties.h"
#include "jc/Container/ListStack.h"
#include "jc/Container/CMessage.h"
#include "jc/Container/DataMap.h"

#include "jc/Primitives/Atomic.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringView.h"
#include "jc/Primitives/StaticString.h"
#include "jc/Primitives/RefCountObjectPtr.h"

#include "jc/FileSystem/File.h"
#include "jc/FileSystem/FileStream.h"
#include "jc/FileSystem/Directory.h"
#include "jc/FileSystem/Path.h"

#include "jc/Encoding/CodePage.h"

#include "jc/Pool/IndexedMemoryPool.h"
#include "jc/Pool/AccessibleObjectPool.h"
#include "jc/Pool/ObjectPool.h"

#include "jc/Pattern/Singleton.h"

#include "jc/Sync/SpinLock.h"
#include "jc/Sync/WaitHandle.h"
#include "jc/Sync/ConditionVariable.h"
#include "jc/Sync/ManualResetEvent.h"
#include "jc/Sync/AutoResetEvent.h"

#include "jc/Logger/ConsoleLogger.h"

#include "jc/Wrapper/WinApi.h"
#include "jc/Wrapper/CRuntime.h"

#include "jc/Utils/Console.h"
#include "jc/Utils/ConsoleMenuItem.h"

#include "jc/Threading/Thread.h"
#include "jc/Threading/ThreadPool.h"
#include "jc/Threading/ThreadLocal.h"
#include "jc/Threading/Scheduler.h"

#include "jc/_Extern/Extern_Console.h"
#include "jc/_Extern/Extern_Random.h"

#include <vector>
#include <map>
#include <set>
#include <list>
#include <iostream>
#include <sstream>
#include <unordered_map>

USING_NS_JC;
USING_NS_STD;

// ============================================================
// Test Functions
// ============================================================

int call_01_AsmStudy(int _argc, char** _argv);
int call_02_HeapStudy(int _argc, char** _argv);
int call_03_RSPGame(int _argc, char** _argv);
int call_04_CoStack(int _argc, char** _argv);
int call_05_PageGuard(int _argc, char** _argv);



