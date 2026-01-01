/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:32:27 PM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "UIRootGroupManager.h"

#include <sgcl/Define_UI.h>
#include <sgcl/UI_Login.h>
#include <sgcl/UI_Inventory.h>
#include <sgcl/UI_Test.h>
#include <sgcl/UI_ChannelSelect.h>

// TODO: 문제점: 기본 해상도로 두고 확대하면 텍스쳐는 당연히 흐릿해지는게 맞는데 라벨(글자)도 흐릿해지는데..
// 라벨 스케일링시 자동으로 스케일된 크기만큼 폰트 크기가 재조정될 수 있어야한다.

//////////////////////////////////////////////////////////////////////////////////////////
template <typename TMasterGroup>
TMasterGroup* CreateRootGroup(UIRootGroupManager* _pParent, const UIGroupElemInfo* _pMasterGroupInfo)
{
	DataManager* pDataManager = DataManager::Get();
	UIElementInfo* pInfo = pDataManager->GetUIElementInfo(_pMasterGroupInfo->code_);
	jc_assert_msg(pInfo->type_ == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	TMasterGroup* pGroup = dbg_new TMasterGroup(static_cast<UIGroupInfo*>(pInfo));
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	pGroup->SetRelativePosition(_pMasterGroupInfo->pos_.x, _pMasterGroupInfo->pos_.y);
	pGroup->InitChildren();

	// 그룹의 자식 생성은 무조건 그룹의 포지션이 결졍되고나서 수행해줘야함. 그룹의 자식들을 초기화하면서
	// 자식들은 부모의 위치에 대한 상대적 위치로 결정되기 때문이다.
	_pParent->AddRootGroup(pGroup);
	return pGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroupManager::UIRootGroupManager(UIGroupInfo* _pInfo)
: pInfo_(_pInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroupManager::~UIRootGroupManager()
{
	for (int i = 0; i < rootGroupList_.Size(); ++i)
		CC_SAFE_RELEASE(rootGroupList_[i]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIRootGroupManager::Init()
{
	jc::HashMap<int, UIGroupElemInfo*> masterGroupInfoMap(pInfo_->infoList_.Size());

	for (int i = 0; i < pInfo_->infoList_.Size(); ++i)
	{
		UIGroupElemInfo& elemInfo = pInfo_->infoList_[i];
		masterGroupInfoMap.Insert(elemInfo.code_, &elemInfo);
	}

	CreateRootGroup<UI_Login>(this, masterGroupInfoMap[GROUP_UI_LOGIN]);
	CreateRootGroup<UI_Inventory>(this, masterGroupInfoMap[GROUP_UI_INVENTORY]);
	CreateRootGroup<UI_Test>(this, masterGroupInfoMap[GROUP_UI_TEST]);
	CreateRootGroup<UI_ChannelSelect>(this, masterGroupInfoMap[GROUP_UI_CHANNEL]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIRootGroupManager::AddRootGroup(UIRootGroup* _pGroup)
{
	rootGroupList_.PushBack(_pGroup);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIRootGroupManager::ForEach(const jc::Action<UIRootGroup*>& _action)
{
	rootGroupList_.ForEach(_action);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroupManager* UIRootGroupManager::CreateRetain()
{
	UIGroupInfo* pGroupInfo = static_cast<UIGroupInfo*>(sg::DataManager->GetUIElementInfo(GROUP_MASTER));
	UIRootGroupManager* pRoot = dbg_new UIRootGroupManager(pGroupInfo);
	pRoot->Init();
	pRoot->autorelease();
	pRoot->retain();
	return pRoot;
}
