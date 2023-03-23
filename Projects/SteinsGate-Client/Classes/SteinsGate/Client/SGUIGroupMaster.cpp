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
#include <SteinsGate/Client/UI_Login.h>
#include <SteinsGate/Client/UI_Inventory.h>

template <typename TMasterGroup>
TMasterGroup* createMasterGroup(SGUIGroupMaster* parent, const SGUIGroupElemInfo* masterGroupInfo) {
	SGDataManager* pDataManager = SGDataManager::get();
	SGUIElementInfo* pInfo = pDataManager->getUIElementInfo(masterGroupInfo->Code);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	TMasterGroup* pGroup = dbg_new TMasterGroup(static_cast<SGUIGroupInfo*>(pInfo));
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	pGroup->setPositionRelative(masterGroupInfo->Pos.x, masterGroupInfo->Pos.y);
	pGroup->initChildren();

	// 그룹의 자식 생성은 무조건 그룹의 포지션이 결졍되고나서 수행해줘야함. 그룹의 자식들을 초기화하면서
	// 자식들은 부모의 위치에 대한 상대적 위치로 결정되기 때문이다.
	parent->addMasterGroup(pGroup);

	return pGroup;
}


SGUIGroupMaster::SGUIGroupMaster(SGUIGroupInfo* info)
	: m_pInfo(info)
{}

SGUIGroupMaster::~SGUIGroupMaster() {
	for (int i = 0; i < m_vMasterGroupList.Size(); ++i)
		CC_SAFE_RELEASE(m_vMasterGroupList[i]);
}

void SGUIGroupMaster::init() {
	SGHashMap<int, SGUIGroupElemInfo*> hMasterGroupInfoMap(m_pInfo->InfoList.Size());

	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		SGUIGroupElemInfo& elemInfo = m_pInfo->InfoList[i];
		hMasterGroupInfoMap.Insert(elemInfo.Code, &elemInfo);
	}

	createMasterGroup<UI_Login>(this, hMasterGroupInfoMap[GROUP_UI_LOGIN]);
	createMasterGroup<UI_Inventory>(this, hMasterGroupInfoMap[GROUP_UI_INVENTORY]);
}

void SGUIGroupMaster::addMasterGroup(SGUIMasterGroup* group) {
	m_vMasterGroupList.PushBack(group);
}

void SGUIGroupMaster::forEach(const SGActionFn<SGUIMasterGroup*>& action) {
	m_vMasterGroupList.Extension().ForEach(action);
}

SGUIGroupMaster* SGUIGroupMaster::createRetain() {
	SGUIGroupInfo* pGroupInfo = static_cast<SGUIGroupInfo*>(CoreDataManager_v->getUIElementInfo(GROUP_MASTER));
	SGUIGroupMaster* pMaster = dbg_new SGUIGroupMaster(pGroupInfo);
	pMaster->init();
	pMaster->autorelease();
	pMaster->retain();
	return pMaster;
}
