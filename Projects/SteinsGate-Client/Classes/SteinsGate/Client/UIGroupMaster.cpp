/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:32:27 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIGroupMaster.h"

#include <SteinsGate/Client/UIDefine.h>
#include <SteinsGate/Client/UI_Login.h>
#include <SteinsGate/Client/UI_Inventory.h>
#include <SteinsGate/Client/UI_Test.h>

// TODO: 문제점: 기본 해상도로 두고 확대하면 텍스쳐는 당연히 흐릿해지는게 맞는데 라벨(글자)도 흐릿해지는데..
// 라벨 스케일링시 자동으로 스케일된 크기만큼 폰트 크기가 재조정될 수 있어야한다.

template <typename TMasterGroup>
TMasterGroup* createMasterGroup(UIGroupMaster* parent, const UIGroupElemInfo* masterGroupInfo) {
	DataManager* pDataManager = DataManager::Get();
	UIElementInfo* pInfo = pDataManager->getUIElementInfo(masterGroupInfo->Code);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	TMasterGroup* pGroup = dbg_new TMasterGroup(static_cast<UIGroupInfo*>(pInfo));
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	pGroup->setRelativePosition(masterGroupInfo->Pos.x, masterGroupInfo->Pos.y);
	pGroup->initChildren();

	// 그룹의 자식 생성은 무조건 그룹의 포지션이 결졍되고나서 수행해줘야함. 그룹의 자식들을 초기화하면서
	// 자식들은 부모의 위치에 대한 상대적 위치로 결정되기 때문이다.
	parent->addMasterGroup(pGroup);
	return pGroup;
}


UIGroupMaster::UIGroupMaster(UIGroupInfo* info)
	: m_pInfo(info)
{}

UIGroupMaster::~UIGroupMaster() {
	for (int i = 0; i < m_vMasterGroupList.Size(); ++i)
		CC_SAFE_RELEASE(m_vMasterGroupList[i]);
}

void UIGroupMaster::init() {
	SGHashMap<int, UIGroupElemInfo*> hMasterGroupInfoMap(m_pInfo->InfoList.Size());

	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		UIGroupElemInfo& elemInfo = m_pInfo->InfoList[i];
		hMasterGroupInfoMap.Insert(elemInfo.Code, &elemInfo);
	}

	createMasterGroup<UI_Login>(this, hMasterGroupInfoMap[GROUP_UI_LOGIN]);
	createMasterGroup<UI_Inventory>(this, hMasterGroupInfoMap[GROUP_UI_INVENTORY]);
	createMasterGroup<UI_Test>(this, hMasterGroupInfoMap[GROUP_UI_TEST]);
}

void UIGroupMaster::addMasterGroup(UIMasterGroup* group) {
	m_vMasterGroupList.PushBack(group);
}

void UIGroupMaster::forEach(const SGActionFn<UIMasterGroup*>& action) {
	m_vMasterGroupList.Extension().ForEach(action);
}

UIGroupMaster* UIGroupMaster::createRetain() {
	UIGroupInfo* pGroupInfo = static_cast<UIGroupInfo*>(CoreDataManager_v->getUIElementInfo(GROUP_MASTER));
	UIGroupMaster* pMaster = dbg_new UIGroupMaster(pGroupInfo);
	pMaster->init();
	pMaster->autorelease();
	pMaster->retain();
	return pMaster;
}
