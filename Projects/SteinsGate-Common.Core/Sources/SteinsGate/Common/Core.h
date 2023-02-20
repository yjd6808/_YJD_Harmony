/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:04:30 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Assert.h>
#include <JCore/TypeTraits.h>
#include <JCore/Define.h>
#include <JCore/Functional.h>
#include <JCore/Math.h>
#include <JCore/Time.h>
#include <JCore/Stream.h>
#include <JCore/Bit.h>

#include <JCore/Primitives/StaticString.h>

#include <JCore/Utils/Concatenation.h>
#include <JCore/Utils/Console.h>

#include <JCore/Threading/Thread.h>
#include <JCore/FileSystem/File.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Directory.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Container/LinkedList.h>

#include <SteinsGate/Common/SGStruct.h>
#include <SteinsGate/Common/SGConfig.h>
#include <SteinsGate/Common/LoggerDefine.h>
#include <SteinsGate/Common/Logo.h>

#include <json.h>
#include <fstream>
#include <set>
#include <map>

#define USING_NS_JS		using namespace Json

#define SG_CACHELINE_SIZE		64
#define SG_FLT_EPSILON			0.0001f
#define SG_PI					3.141592f

using SGConsole = JCore::Console;
using SGString = JCore::String;

template <Int32U Size>
using SGStaticString = JCore::StaticString<Size>;
using SGPath = JCore::Path;
using SGFile = JCore::File;
using SGDirectory = JCore::Directory;
using SGStringUtil = JCore::StringUtil;

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
using SGHashMap = JCore::HashMap<TKey, TValue, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGVector = JCore::Vector<T, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGLinkedList = JCore::LinkedList<T, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGArrayQueue = JCore::ArrayQueue<T, TAllocator>;

template <typename T, typename Predicate = std::less<T>>
using SGSet = std::set<T, Predicate>;

template <typename TKey, typename TValue, typename Predicate = std::less<TKey>>
using SGMap = std::map<TKey, TValue, Predicate>;

using SGMath = JCore::Math;

template <typename... Args>
using SGActionFn = JCore::Action<Args...>;

template <typename R, typename... Args>
using SGFuncFn = JCore::Func<R, Args...>;

template <typename... Args>
using SGPredicateFn = JCore::Predicate<Args...>;

using SGThread = JCore::Thread;

using SGStream = JCore::Stream;
using SGStreamPtr = JCore::StreamPtr;
using SGFileStream = JCore::FileStream;
using SGFileStreamPtr = JCore::FileStreamPtr;

using SGConsoleColor = JCore::ConsoleColor;
using SGDateTime = JCore::DateTime;

using SGNormalLock = JCore::NormalLock;
using SGRecursiveLock = JCore::RecursiveLock;

template <typename... TArgs>
using SGTuple = JCore::Tuple<TArgs...>;


template <typename T, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) AlignedVector : public SGVector<T, TAllocator> {};

template <typename T, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) AlignedLinkedList : public SGLinkedList<T, TAllocator> {};

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) AlignedHashMap : public SGHashMap<TKey, TValue, TAllocator> {};

// 모든 프로젝트에서 사용하는 변수


void InitializeCommonCore();
void FinalizeCommonCore();


