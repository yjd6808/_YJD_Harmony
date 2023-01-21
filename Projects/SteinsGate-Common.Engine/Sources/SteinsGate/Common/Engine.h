/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 3:15:13 AM
 * =====================
 *
 */


#pragma once

#include <cocos2d.h>
#include <cocos/ui/CocosGUI.h>

#include <JCore/Assert.h>
#include <JCore/TypeTraits.h>
#include <JCore/Define.h>

#include <JCore/Utils/Console.h>
#include <JCore/Threading/Thread.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Container/LinkedList.h>


#define USING_NS_JC using namespace JCore
#define USING_NS_JN using namespace JNetwork
#define USING_NS_JS using namespace Json
#define USING_NS_CCUI using namespace cocos2d::ui

using SGConsole = JCore::SafeConsole;

using SGString = JCore::String;

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
using SGHashMap = JCore::HashMap<TKey, TValue, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGVector = JCore::Vector<T, TAllocator>;