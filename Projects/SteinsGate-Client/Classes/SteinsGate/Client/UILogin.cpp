/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "UILogin.h"

#include "SGUIGroup.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>


USING_NS_CC;
USING_NS_JC;


UILogin* UILogin::createRetain() {
	UILogin* pGroup = dbg_new UILogin();
	SGDataManager* pDataManager = SGDataManager::get();
	SGUIElementInfo* pInfo = pDataManager->getUIElementInfo(UI_GROUP_LOGIN);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	pGroup->m_pInfo = (SGUIGroupInfo*)pInfo;

	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	return pGroup;
}

bool UILogin::init() {

	if (!SGUIGroup::init()) {
		return false;
	}

	return true;
}


