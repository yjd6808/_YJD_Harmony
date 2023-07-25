/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:18 AM
 * =====================
 *
 */




#include "Tutturu.h"
#include "ChannelSelectScene.h"

#include <SteinsGate/Client/WorldScene.h>

#include "FontPackage.h"

USING_NS_CC;
USING_NS_JC;

// ==============================================================
// 스태틱
// ==============================================================

SGChannelSelectScene* SGChannelSelectScene::create() {
	SGChannelSelectScene* scene = dbg_new SGChannelSelectScene();

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

SGChannelSelectScene::SGChannelSelectScene() {}

bool SGChannelSelectScene::init() {
	if (!SceneBase::init()) {
		return false;
	}

	auto a = Sprite::create("rect.png");
	a->setAnchorPoint(Vec2::ZERO);
	a->setPosition({ 200.0f, 200.0f });
	this->addChild(a);

	SGText* text = SGText::create("abcd", CoreFont_v->getFontName(0).Source(), 16);
	text->setPosition({ 300, 300 });
	this->addChild(text);

	return true;
}


void SGChannelSelectScene::update(float dt) {
	
}

void SGChannelSelectScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_F8) {
		WorldScene::get()->reserveScene(SceneType::Game);
	} else if (keyCode == EventKeyboard::KeyCode::KEY_F7) {
		WorldScene::get()->reserveScene(SceneType::Login);
	}
}

void SGChannelSelectScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {}