/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:32:27 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIGroupMaster.h"

#include <SteinsGate/Client/SGUIDefine.h>
#include <SteinsGate/Client/UILogin.h>

template <typename TMasterGroup>
TMasterGroup* createMasterGroup(SGUIGroup* parent, int code) {
	SGDataManager* pDataManager = SGDataManager::get();
	SGUIElementInfo* pInfo = pDataManager->getUIElementInfo(code);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	TMasterGroup* pGroup = dbg_new TMasterGroup(parent, static_cast<SGUIGroupInfo*>(pInfo));
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	return pGroup;
}


SGUIGroupMaster::SGUIGroupMaster(SGUIGroupInfo* info)
	: SGUIGroup(nullptr, info) {
}

SGUIGroupMaster::~SGUIGroupMaster() {
	for (int i = 0; i < vChildren.Size(); ++i)
		CC_SAFE_RELEASE(vChildren[i]);
}

bool SGUIGroupMaster::init() {
	if (!SGUIElement::init())
		return false;

	setPosition(0, 0);
	setContentSize(m_pInfo->Size);

	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		SGUIGroupElemInfo& elemInfo = m_pInfo->InfoList[i];
		addUIGroup(elemInfo);
	}

	return true;
}

void SGUIGroupMaster::addUIGroup(const SGUIGroupElemInfo& groupElemInfo) {
	SGUIGroup* pGroup = createMasterGroup<UILogin>(this, groupElemInfo.Code);
	pGroup->setPositionRelative(groupElemInfo.Pos.x, groupElemInfo.Pos.y);
	pGroup->initChildren();
	// 그룹의 자식 생성은 무조건 그룹의 포지션이 결졍되고나서 수행해줘야함. 그룹의 자식들을 초기화하면서
	// 자식들은 부모의 위치에 대한 상대적 위치로 결정되기 때문이다.
	vChildren.PushBack(pGroup);
}



SGUIGroupMaster* SGUIGroupMaster::createRetain() {
	SGUIGroupInfo* pGroupInfo = static_cast<SGUIGroupInfo*>(CoreDataManager_v->getUIElementInfo(GROUP_MASTER));
	SGUIGroupMaster* pMaster = dbg_new SGUIGroupMaster(pGroupInfo);
	pMaster->init();
	pMaster->autorelease();
	pMaster->retain();
	return pMaster;
}
