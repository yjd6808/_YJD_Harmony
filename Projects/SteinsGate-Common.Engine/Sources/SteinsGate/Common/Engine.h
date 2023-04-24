/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 3:15:13 AM
 * =====================
 *
 */


#pragma once

#include <cocos2d.h>
#include <cocos/ui/CocosGUI.h>

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Color.h>

#define USING_NS_CCUI	using namespace cocos2d::ui

// 코코스
using SGNode = cocos2d::Node;
using SGLayer = cocos2d::Layer;
using SGLayerColor = cocos2d::LayerColor;
using SGSprite = cocos2d::Sprite;
using SGScale9Sprite = cocos2d::ui::Scale9Sprite;
using SGVec2 = cocos2d::Vec2;
using SGSize = cocos2d::Size;
using SGRect = cocos2d::Rect;
using SGSpriteFrame = cocos2d::SpriteFrame;
using SGProgressTimer = cocos2d::ProgressTimer;
using SGTexture = cocos2d::Texture2D;
using SGDrawNode = cocos2d::DrawNode;
using SGScene = cocos2d::Scene;
using SGEventCustom = cocos2d::EventCustom;
using SGText = cocos2d::ui::Text;
using SGEditBox = cocos2d::ui::EditBox;
using SGEditBoxImplCommon = cocos2d::ui::EditBoxImplCommon;
using SGEditBoxImplWin = cocos2d::ui::EditBoxImplWin;
using SGEditBoxEndAction = cocos2d::ui::EditBoxDelegate::EditBoxEndAction;
using SGLabel = cocos2d::Label;
using SGEventKeyboard = cocos2d::EventKeyboard;
using SGEventListenerKeyboard = cocos2d::EventListenerKeyboard;
using SGEventListenerMouse = cocos2d::EventListenerMouse;
using SGEvent = cocos2d::Event;
using SGEventMouse = cocos2d::EventMouse;
using SGMouseEventList = SGEventList<SGEventMouse*>;
using SGRandom = cocos2d::RandomHelper;
using SGDirector = cocos2d::Director;
using SGColor3B = cocos2d::Color3B;
using SGColor4B = cocos2d::Color4B;
using SGColor4F = cocos2d::Color4F;
using SGColorList = ColorList;
using SGInputMode = cocos2d::ui::EditBox::InputMode;
using SGCheckBox = cocos2d::ui::CheckBox;
