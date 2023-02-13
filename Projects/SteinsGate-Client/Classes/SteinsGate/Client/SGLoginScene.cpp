/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:51 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGLoginScene.h"

#include <SteinsGate/Client/SGWorldScene.h>

USING_NS_CC;
USING_NS_JC;

// ==============================================================
// 스태틱
// ==============================================================

SGLoginScene* SGLoginScene::create() {
	SGLoginScene*  scene = new (std::nothrow) SGLoginScene();
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

SGLoginScene::SGLoginScene()
	: SGSceneBase(SceneType::Login)
{}

bool SGLoginScene::init() {
	if (!SGSceneBase::init()) {
		return false;
	}


	return true;
}


void SGLoginScene::update(float dt) {
	
}

void SGLoginScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
		SGWorldScene::get()->changeScene(SceneType::ServerSelect);
	}
}

void SGLoginScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {}