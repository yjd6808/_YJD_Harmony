/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:51 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "LoginScene.h"

#include <SteinsGate/Client/WorldScene.h>
#include <SteinsGate/Client/UILayer.h>
#include <SteinsGate/Client/UIDefine.h>

USING_NS_CC;
USING_NS_JC;

// ==============================================================
// 스태틱
// ==============================================================

SGLoginScene* SGLoginScene::create() {
	SGLoginScene*  scene = dbg_new SGLoginScene();
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
	: SceneBase()
{}

bool SGLoginScene::init() {
	if (!SceneBase::init()) {
		return false;
	}

	m_pUILayer->addUIGroup(GROUP_UI_LOGIN);
	return true;
}


void SGLoginScene::update(float dt) {
	
}

void SGLoginScene::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_F8) {
		WorldScene::get()->reserveScene(SceneType::ChannelSelect);
	} else if (keyCode == EventKeyboard::KeyCode::KEY_F7) {
		WorldScene::get()->reserveScene(SceneType::Game);
	}
}

void SGLoginScene::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {}