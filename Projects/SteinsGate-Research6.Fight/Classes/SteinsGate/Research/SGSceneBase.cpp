/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:41:04 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGSceneBase.h"

SGSceneBase::SGSceneBase(SceneType_t type)
	: m_eType(type) {}

SGSceneBase::~SGSceneBase() {
	Log("[%s] 씬 해제\n", SceneType::Name[m_eType]);
}
