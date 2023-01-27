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
#include <JCore/Primitives/StringUtil.h>

#include <JCore/Utils/Console.h>
#include <JCore/Threading/Thread.h>
#include <JCore/FileSystem/File.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/FileSystem/Directory.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Container/LinkedList.h>


#define USING_NS_JC using namespace JCore
#define USING_NS_JN using namespace JNetwork
#define USING_NS_JS using namespace Json
#define USING_NS_CCUI using namespace cocos2d::ui

#define SG_FLT_EPSILON	0.0001f

using SGConsole = JCore::SafeConsole;
using SGString = JCore::String;
using SGPath = JCore::Path;
using SGStringUtil = JCore::StringUtil;

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
using SGHashMap = JCore::HashMap<TKey, TValue, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGVector = JCore::Vector<T, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGArrayQueue = JCore::ArrayQueue<T, TAllocator>;

template <typename T, typename Predicate = std::less<T>>
using SGSet = std::set<T, Predicate>;

template <typename TKey, typename TValue, typename Predicate = std::less<TKey>>
using SGMap = std::map<TKey, TValue, Predicate>;



// 코코스
using SGNode = cocos2d::Node;
using SGLayer = cocos2d::Layer;
using SGLayerColor = cocos2d::LayerColor;
using SGSprite = cocos2d::Sprite;
using SGVec2 = cocos2d::Vec2;
using SGSize = cocos2d::Size;
using SGRect = cocos2d::Rect;
using SGSpriteFrame = cocos2d::SpriteFrame;
using SGTexture = cocos2d::Texture2D;
using SGDrawNode = cocos2d::DrawNode;
using SGScene = cocos2d::Scene;
using SGText = cocos2d::ui::Text;
using SGEventKeyboard = cocos2d::EventKeyboard;
using SGEvent = cocos2d::Event;
using SGRandom = cocos2d::RandomHelper;