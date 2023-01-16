/*
 * 작성자 : 윤정도
 */

#include "TestLayer.h"

#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>
#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Research/CharacterSprite.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

#include <JCore/Utils/Console.h>



using namespace cocos2d;
using namespace cocos2d::ui;
using namespace JCore;




bool TestLayer::init() {

	if (!Layer::init()) {
		return false;
	}


	Rect a;
	Rect b;

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(TestLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(TestLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
	return true;
}

void TestLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

}




void TestLayer::update(float delta) {
}

