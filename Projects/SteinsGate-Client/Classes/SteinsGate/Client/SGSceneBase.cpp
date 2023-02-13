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

SGSceneBase::SGSceneBase(SceneType_t type)
	: m_eType(type) {}

SGSceneBase::~SGSceneBase() {
	Log("[%s] 씬 소멸\n", SceneType::Name[m_eType]);
}

bool SGSceneBase::init() {
	Log("[%s] 씬 init\n", SceneType::Name[m_eType]);
	return Scene::init();
}
void SGSceneBase::onEnter() {
	Log("[%s] 씬 onEnter\n", SceneType::Name[m_eType]);
}
void SGSceneBase::onExit() {
	Log("[%s] 씬 onExit\n", SceneType::Name[m_eType]);
}

void SGSceneBase::onEnterTransitionDidFinish() {
	Log("[%s] 씬 onEnterTransitionDidFinish\n", SceneType::Name[m_eType]);
}

void SGSceneBase::onExitTransitionDidStart() {
	Log("[%s] 씬 onExitTransitionDidStart\n", SceneType::Name[m_eType]);
}
