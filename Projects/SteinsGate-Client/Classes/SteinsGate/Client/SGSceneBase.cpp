/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:41:04 AM
 * =====================
 * 씬 호출 과정 테스트
 *
 *  1. 생성자
 *	2. init
 *	3. onEnter
 *	4. onEnterTransitionDidFinish
 *	5. removechild로 씬 해제
 *	6. onExitTransitionDidStart
 *  7. onExit
 *	8. 메모리 해제
 */


#include "Tutturu.h"
#include "SGSceneBase.h"

#include <SteinsGate/Client/SGWorldScene.h>
#include <SteinsGate/Client/SGUILayer.h>

USING_NS_CC;
USING_NS_JC;

SGSceneBase::SGSceneBase() {}

SGSceneBase::~SGSceneBase() {
}

bool SGSceneBase::init() {
	_LogDebug_("%s 씬 초기화", SceneType::Name[getType()]);

	if (!Scene::init())
		return false;

	m_pWorldScene = SGWorldScene::get();
	m_pUILayer = m_pWorldScene->getUILayer();

	auto a = Sprite::create("hud.png");
	a->setAnchorPoint(Vec2::ZERO);
	a->setScaleX(1.0f);
	a->setScaleY(1.0f);
	this->addChild(a);

	return true;
}
void SGSceneBase::onEnter() {
	_LogDebug_("%s 씬을 시작", SceneType::Name[getType()]);
}
void SGSceneBase::onExit() {
	_LogDebug_("%s 씬을 종료", SceneType::Name[getType()]);
	Scene::onExit();
}

void SGSceneBase::onEnterTransitionDidFinish() {
}

void SGSceneBase::onExitTransitionDidStart() {
}

