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
#include <JCore/Primitives/StringUtil.h>

#include <JCore/Utils/Console.h>
#include <JCore/Threading/Thread.h>
#include <JCore/FileSystem/File.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/FileSystem/Directory.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Container/LinkedList.h>

#include <SteinsGate/Common/SGStruct.h>
#include <SteinsGate/Common/SGConfig.h>

#include <json.h>
#include <set>
#include <map>

#define USING_NS_JS		using namespace Json

#define SG_CACHELINE_SIZE		64
#define SG_FLT_EPSILON			0.0001f
#define SG_PI					3.141592f

using SGConsole = JCore::SafeConsole;
using SGString = JCore::String;
using SGPath = JCore::Path;
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

template <typename T, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) AlignedVector : public SGVector<T, TAllocator> {};

template <typename T, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) AlignedLinkedList : public SGLinkedList<T, TAllocator> {};

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) AlignedHashMap : public SGHashMap<TKey, TValue, TAllocator> {};

void InitializeCommonCore();
void FinalizeCommonCore();
