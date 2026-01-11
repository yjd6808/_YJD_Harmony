/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:11:04 PM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "UIManager.h"

#include <sg/_Core/AppConfig.h>
#include <sg/_Struct/SteinsGate_Client.h>

#include <sgcl/UIGroup.h>
#include <sgcl/UIRootGroup.h>

#include <sgcl/Define_UI.h>
#include <sgcl/PopupManager.h>

#include <sgcl/UI_Inventory.h>
#include <sgcl/UI_Login.h>
#include <sgcl/UI_Popup.h>
#include <sgcl/UI_Test.h>
#include <sgcl/UI_ChannelSelect.h>
#include <sgcl/ImagePackManager.h>


USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIManager::UIManager()
: pRootGroupMgr_(nullptr)
, loadedUITexture_(1024) // 창을 64개까지 만들일이 있을려나 ㅋㅋ
, uiElementMap_(512)
, masterUIGroups_(64)
, popup_(*PopupManager::Get())
//////////////////////////////////////////////////////////////////////////////////////////
{
	pInventory_ = nullptr;
	pLogin_ = nullptr;
	pTest_ = nullptr;
	pChannelSelect_ = nullptr;

}

//////////////////////////////////////////////////////////////////////////////////////////
UIManager::~UIManager()
{
	CC_SAFE_RELEASE(pRootGroupMgr_);

	// 마스터 UI 그룹만 제거해주면 된다.
	// 어차피 내부 자식들은 모두 마스터 UI 그룹에 addChild 되어 있기 때문에
	// 이녀석만 제거하면 도미노 마냥 다 제거댐
	masterUIGroups_.ForEachValue([](UIGroup* _group)
	{
		CC_SAFE_RELEASE(_group);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
// : 루트 유아이 그룹 등록
void UIManager::Init()
{
	pRootGroupMgr_ = UIRootGroupManager::CreateRetain();
	pRootGroupMgr_->ForEach([this](UIRootGroup* _pRootGroup) { RegisterRootGroup(_pRootGroup); });
	pRootGroupMgr_->ForEach([this](UIRootGroup* _pRootGroup) { _pRootGroup->OnInit(); });

	InitPublic();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RegisterRootGroup(UIRootGroup* _pGroup)
{
	_pGroup->retain(); // 그룹마스터에서 생성/관리하기때문에 리테인했고 UIManager에서도 마스터를 참조하므로, 리테인해줘야함. 빼먹고있었넹
	uiElementMap_.Insert(_pGroup->GetCode(), _pGroup);
	masterUIGroups_.Insert(_pGroup->GetCode(), _pGroup);

	_pGroup->ForEachRecursive([this](UIElement* _pUIElement)
	{
		const int elementCode = _pUIElement->GetCode();
		const bool inserted = uiElementMap_.Insert(elementCode, _pUIElement);
		if (!inserted)
		{
			_LogWarn_("이미 UI 엘리먼트(%d)가 그룹내 포함되어 있습니다.", elementCode);
		}
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RegisterUITexture(SgaResourceIndex _index)
{
	if (_index.un_.frameIndex_ == InvalidValue_v)
	{
		return;
	}

	const bool inserted = loadedUITexture_.Insert(_index.value_, _index);

	if (inserted)
	{
		g_cImagePackMgr.LogTexture("로드", _index, LoggerAbstract::eDebug);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::UnloadAll()
{
	ImagePackManager* pPackManager = ImagePackManager::Get();

	// 이미지 텍스쳐 모두 릴리즈
	masterUIGroups_.ForEachValue([](UIGroup* _group)
	{
		_group->Unload();
	});

	// 관련 캐쉬, 팩 모두 언로드
	loadedUITexture_.ForEachValue([pPackManager](SgaResourceIndex& _resourceIndex)
	{
		pPackManager->ReleaseFrameTexture(_resourceIndex);
		pPackManager->UnloadPackData(_resourceIndex.un_.sgaIndex_);
	});
	loadedUITexture_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::OnUpdate(float _dt)
{
	CallUiElementsUpdateCallback(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::CallUiElementsUpdateCallback(float _dt)
{
	uiElementsUpdateEvent_.ForEach([&_dt](Pair<UIElement*, jc::Event<UIElement*, float>>& _elementEventPair)
	{
		_elementEventPair.value_.Invoke((UIElement*)_elementEventPair.key_, (float)_dt);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Draginit(const DragState& _state)
{
	dragState_ = _state;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::DragEnter(const cc::EventMouse* _pMouseEvent)
{
	UIElement* pDragElement = dragState_.pTargetElement_;
	const Vec2 dragDelta = _pMouseEvent->getStartCursorPos() - dragState_.startCursorPosition_;

	pDragElement->setPosition(dragState_.startElementPosition_ + dragDelta);
	pDragElement->GetRootGroup()->OnDragEnter(pDragElement, dragState_);

	dragState_.dragDelta_ = dragDelta;
	dragState_.isDragging_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::DragMove(const cc::EventMouse* _pMouseEvent)
{
	UIElement* pDragElement = dragState_.pTargetElement_;
	const Vec2 dragDelta = _pMouseEvent->getStartCursorPos() - dragState_.startCursorPosition_;

	pDragElement->setPosition(dragState_.startElementPosition_ + dragDelta);
	pDragElement->GetRootGroup()->OnDragMove(pDragElement, dragState_);

	dragState_.dragDelta_ = dragDelta;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::DragEnd()
{
	dragState_.pHostElement_ = nullptr;
	dragState_.pTargetElement_ = nullptr;
	dragState_.isDragging_ = false;
	dragState_.dragDelta_ = {};
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroup* UIManager::GetRootGroup(int _groupCode)
{
	if (!masterUIGroups_.Exist(_groupCode))
	{
		_LogWarn_("%d 마스터 UI 그룹이 존재하지 않습니다.", _groupCode);
		return nullptr;
	}

	return masterUIGroups_[_groupCode];
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElement* UIManager::GetElement(int _elementCode)
{
	if (!uiElementMap_.Exist(_elementCode))
	{
		return nullptr;
	}

	return uiElementMap_[_elementCode];
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* UIManager::CreateUITexture(int _sga, int _img, int _frame, bool _linearDodge /* = false  */)
{
	ImagePack* pPack = g_cImagePackMgr.GetPackUnsafe(_sga);

	if (pPack == nullptr)
	{
		g_cImagePackMgr.LogTexture("UIManager::createUITexture()", SgaResourceIndex{ _sga, _img, _frame },
		                                       LoggerAbstract::eWarn);
		return SpriteFrameTexture::GetDefault();
	}

	FrameTexture* pTexture = pPack->CreateFrameTexture(_img, _frame, _linearDodge);
	RegisterUITexture({ _sga, _img, _frame });
	return pTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* UIManager::CreateUITextureRetained(int _sga, int _img, int _frame, bool _linearDodge)
{
	FrameTexture* pTexture = CreateUITexture(_sga, _img, _frame, _linearDodge);
	pTexture->retain();
	return pTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::InitPublic()
{
	pLogin_ = (UI_Login*)GetRootGroup(GROUP_UI_LOGIN);
	pInventory_ = (UI_Inventory*)GetRootGroup(GROUP_UI_INVENTORY);
	pTest_ = (UI_Test*)GetRootGroup(GROUP_UI_TEST);
	pChannelSelect_ = (UI_ChannelSelect*)GetRootGroup(GROUP_UI_CHANNEL);
}
