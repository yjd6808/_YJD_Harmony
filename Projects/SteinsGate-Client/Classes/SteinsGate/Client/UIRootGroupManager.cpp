/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:32:27 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIRootGroupManager.h"

#include <SteinsGate/Client/Define_UI.h>
#include <SteinsGate/Client/UI_Login.h>
#include <SteinsGate/Client/UI_Inventory.h>
#include <SteinsGate/Client/UI_Test.h>
#include <SteinsGate/Client/UI_ChannelSelect.h>

// TODO: 문제점: 기본 해상도로 두고 확대하면 텍스쳐는 당연히 흐릿해지는게 맞는데 라벨(글자)도 흐릿해지는데..
// 라벨 스케일링시 자동으로 스케일된 크기만큼 폰트 크기가 재조정될 수 있어야한다.

//////////////////////////////////////////////////////////////////////////////////////////
template <typename TMasterGroup>
TMasterGroup* createMasterGroup(UIRootGroupManager* _pParent, const UIGroupElemInfo* _pMasterGroupInfo)
{
	DataManager* pDataManager = DataManager::Get();
	UIElementInfo* pInfo = pDataManager->getUIElementInfo(_pMasterGroupInfo->Code);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	TMasterGroup* pGroup = dbg_new TMasterGroup(static_cast<UIGroupInfo*>(pInfo));
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	pGroup->SetRelativePosition(_pMasterGroupInfo->Pos.x, _pMasterGroupInfo->Pos.y);
	pGroup->initChildren();

	// 그룹의 자식 생성은 무조건 그룹의 포지션이 결졍되고나서 수행해줘야함. 그룹의 자식들을 초기화하면서
	// 자식들은 부모의 위치에 대한 상대적 위치로 결정되기 때문이다.
	_pParent->addMasterGroup(pGroup);
	return pGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroupManager::UIRootGroupManager(UIGroupInfo* _pInfo)
: info_(_pInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroupManager::~UIRootGroupManager()
{
	for (int i = 0; i < masterGroupList_.Size(); ++i)
		CC_SAFE_RELEASE(masterGroupList_[i]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIRootGroupManager::init()
{
	SGHashMap<int, UIGroupElemInfo*> masterGroupInfoMap(info_->InfoList.Size());

	for (int i = 0; i < info_->InfoList.Size(); ++i)
	{
		UIGroupElemInfo& elemInfo = info_->InfoList[i];
		masterGroupInfoMap.Insert(elemInfo.Code, &elemInfo);
	}

	createMasterGroup<UI_Login>(this, masterGroupInfoMap[GROUP_UI_LOGIN]);
	createMasterGroup<UI_Inventory>(this, masterGroupInfoMap[GROUP_UI_INVENTORY]);
	createMasterGroup<UI_Test>(this, masterGroupInfoMap[GROUP_UI_TEST]);
	createMasterGroup<UI_ChannelSelect>(this, masterGroupInfoMap[GROUP_UI_CHANNEL]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIRootGroupManager::addMasterGroup(UIRootGroup* _pGroup)
{
	masterGroupList_.PushBack(_pGroup);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIRootGroupManager::forEach(const SGActionFn<UIRootGroup*>& _action)
{
	masterGroupList_.ForEach(_action);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroupManager* UIRootGroupManager::createRetain()
{
	UIGroupInfo* pGroupInfo = static_cast<UIGroupInfo*>(Core::DataManager->getUIElementInfo(GROUP_MASTER));
	UIRootGroupManager* pMaster = dbg_new UIRootGroupManager(pGroupInfo);
	pMaster->init();
	pMaster->autorelease();
	pMaster->retain();
	return pMaster;
}
