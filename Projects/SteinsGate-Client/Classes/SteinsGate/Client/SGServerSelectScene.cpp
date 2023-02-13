/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:18 AM
 * =====================
 *
 */




#include "Tutturu.h"
#include "SGServerSelectScene.h"

#include <SteinsGate/Client/SGWorldScene.h>

USING_NS_CC;
USING_NS_JC;

// ==============================================================
// 스태틱
// ==============================================================

SGServerSelectScene* SGServerSelectScene::create() {
	SGServerSelectScene*  scene = new (std::nothrow) SGServerSelectScene();
	if (scene && scene->init()) {
		scene->autorelease();
		return scene;
	}
	CC_SAFE_DELETE(scene);
	return nullptr;
}

// ==============================================================
// 멤버
// ==============================================================

SGServerSelectScene::SGServerSelectScene()
	: SGSceneBase(SceneType::ServerSelect)
{}

bool SGServerSelectScene::init() {
	if (!SGSceneBase::init()) {
		return false;
	}

	SGText* text = SGText::create("서버 셀렉", "nexon_gothic.ttf", 16);
	text->setPosition({ 300, 300 });
	this->addChild(text);

	return true;
}


void SGServerSelectScene::update(float dt) {
	
}

void SGServerSelectScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
		SGWorldScene::get()->changeScene(SceneType::Game);
	}
}

void SGServerSelectScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {}