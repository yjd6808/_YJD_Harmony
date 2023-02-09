/*
 * 작성자 : 윤정도
 */

#include "TestLayer.h"

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkColorFormat.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>
#include <SteinsGate/Common/Core/Npk/NpkImage.h>
#include <SteinsGate/Common/Core/Npk/NpkSprite.h>
#include <SteinsGate/Common/Core/Npk/NpkLoader.h>

#include <SteinsGate/Character.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace JCore;


bool TestLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	// 텍스쳐는 

	// -250, 250
	

	//Character* character = Character::create();
	//character->setPosition(200, 60);
	//Nodes.PushBack(character);
	//character->runAction(
	//	RepeatForever::create(Sequence::create(
	//		EaseOut::create(MoveBy::create(1.5f, Vec2{ 0.0f, 400.0f }), 4.0f),
	//		EaseIn::create(MoveBy::create(1.5f, Vec2{ 0.0f, -400.0f }), 4.0f),
	//		nullptr
	//	))
	//);
	//this->addChild(character);

	Character* character = Character::create();
	character->setPosition(300, 300);
	Nodes.PushBack(character);
	this->addChild(character);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(TestLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(TestLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	
	this->scheduleUpdate();
	return true;
}

void TestLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	// printf("%d %d\n", (int)MainCharacter->getPositionX(), (int)MainCharacter->getPositionY());
	if (keyCode == EventKeyboard::KeyCode::KEY_A) {
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_S) {
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		for (int i = 0; i < Nodes.Size(); i++) {
			Nodes[i]->setPosition(Nodes[i]->getPositionX() - 30.1f, Nodes[i]->getPositionY());
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		for (int i = 0; i < Nodes.Size(); i++) {
			Nodes[i]->setPosition(Nodes[i]->getPositionX() + 30.1f, Nodes[i]->getPositionY());
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
		for (int i = 0; i < Nodes.Size(); i++) {
			Nodes[i]->setPosition(Nodes[i]->getPositionX(), Nodes[i]->getPositionY() + 30.1f);
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
		for (int i = 0; i < Nodes.Size(); i++) {
			Nodes[i]->setPosition(Nodes[i]->getPositionX(), Nodes[i]->getPositionY() - 30.1f);
		}
	}

}

void TestLayer::update(float delta) {

}

