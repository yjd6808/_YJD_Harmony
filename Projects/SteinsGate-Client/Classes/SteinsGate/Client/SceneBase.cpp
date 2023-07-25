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
#include "GameCoreHeader.h"
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

	removeChild(_defaultCamera);
	return true;
}

void SceneBase::onEnter() {
	_LogDebug_("%s 씬을 시작", SceneType::Name[getType()]);
	Scene::onEnter();
}

void SceneBase::onExit() {
	_LogDebug_("%s 씬을 종료", SceneType::Name[getType()]);
	Scene::onExit();
	m_pUILayer->clearUnload();
	CorePackManager_v->releaseAllFrameTexture();
	
}

void SceneBase::onEnterTransitionDidFinish() {
}

void SceneBase::onExitTransitionDidStart() {
}



void SceneBase::removeAllChildren() {

	// Scene::removeAllChildren() 함수는 디폴트 카메라를 삭제하지 않기 때문에
	// 디폴트 카메라도 삭제해주도록 하기 위해서 오버라이딩 구현함

	Node::removeAllChildren();
}

