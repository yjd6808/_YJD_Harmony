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
#include "SceneBase.h"

#include <SteinsGate/Client/WorldScene.h>
#include <SteinsGate/Client/UILayer.h>

USING_NS_CC;
USING_NS_JC;

SceneBase::SceneBase() {}

SceneBase::~SceneBase() {
}

bool SceneBase::init() {
	_LogDebug_("%s 씬 초기화", SceneType::Name[getType()]);

	if (!Scene::init())
		return false;

	m_pWorldScene = WorldScene::get();
	m_pUILayer = m_pWorldScene->getUILayer();

	return true;
}
void SceneBase::onEnter() {
	_LogDebug_("%s 씬을 시작", SceneType::Name[getType()]);
}
void SceneBase::onExit() {
	_LogDebug_("%s 씬을 종료", SceneType::Name[getType()]);
	Scene::onExit();
}

void SceneBase::onEnterTransitionDidFinish() {
}

void SceneBase::onExitTransitionDidStart() {
}

