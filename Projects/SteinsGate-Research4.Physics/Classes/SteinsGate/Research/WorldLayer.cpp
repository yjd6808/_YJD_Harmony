/*
 * 작성자 : 윤정도
 */

#include "WorldLayer.h"

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

#include <JCore/Utils/Console.h>



using namespace cocos2d;
using namespace cocos2d::ui;
using namespace JCore;


bool WorldLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(WorldLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(WorldLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
	return true;
}

void WorldLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

}




void WorldLayer::update(float delta) {
	
}

