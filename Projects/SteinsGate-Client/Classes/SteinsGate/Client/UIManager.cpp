/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:11:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIManager.h"

#include <SteinsGate/Client/UIGroup.h>
#include <SteinsGate/Client/UIRootGroup.h>
#include <SteinsGate/Client/Global.h>

#include <SteinsGate/Client/Define_UI.h>

#include <SteinsGate/Client/UI_Inventory.h>
#include <SteinsGate/Client/UI_Login.h>
#include <SteinsGate/Client/UI_Popup.h>
#include <SteinsGate/Client/UI_Test.h>
#include <SteinsGate/Client/UI_ChannelSelect.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIManager::UIManager()
: master_(nullptr)
, loadedUiTexture_(1024) // 창을 64개까지 만들일이 있을려나 ㅋㅋ
, uiElements_(512)
, masterUiGroups_(64)
//////////////////////////////////////////////////////////////////////////////////////////
{
	Inventory = nullptr;
	Login = nullptr;
	Popup = nullptr;
	Test = nullptr;
	ChannelSelect = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIManager::~UIManager()
{
	CC_SAFE_RELEASE(master_);

	// 마스터 UI 그룹만 제거해주면 된다.
	// 어차피 내부 자식들은 모두 마스터 UI 그룹에 addChild 되어 있기 때문에
	// 이녀석만 제거하면 도미노 마냥 다 제거댐
	masterUiGroups_.ForEachValue([](UIGroup* group)
	{
		CC_SAFE_RELEASE(group);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
// =====================================================
//           루트 유아이 그룹 등록
// =====================================================

void UIManager::init()
{
	master_ = UIRootGroupManager::createRetain();
	master_->forEach([this](UIRootGroup* _pRootGroup) { registerMasterGroup(_pRootGroup); });
	master_->forEach([this](UIRootGroup* _pRootGroup) { _pRootGroup->OnInit(); });

	initPublic();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::registerMasterGroup(UIRootGroup* _pGroup)
{
	_pGroup->retain(); // 그룹마스터에서 생성/관리하기때문에 리테인했고 UIManager에서도 마스터를 참조하므로, 리테인해줘야함. 빼먹고있었넹
	uiElements_.Insert(_pGroup->GetCode(), _pGroup);
	masterUiGroups_.Insert(_pGroup->GetCode(), _pGroup);

	_pGroup->forEachRecursive([this](UIElement* _pUIElement)
	{
		const int elementCode = _pUIElement->GetCode();
		const bool inserted = uiElements_.Insert(elementCode, _pUIElement);
		if (!inserted)
			_LogWarn_("이미 UI 엘리먼트(%d)가 그룹내 포함되어 있습니다.", elementCode);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::registerUITexture(SgaResourceIndex _index)
{
	if (_index.Un.FrameIndex == InvalidValue_v)
	{
		return;
	}

	const bool inserted = loadedUiTexture_.Insert(_index.Value, _index);

	if (inserted)
	{
		Core::Contents.PackManager->logTexture("로드", _index, LoggerAbstract::eDebug);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::unloadAll()
{
	ImagePackManager* pPackManager = ImagePackManager::Get();

	// 이미지 텍스쳐 모두 릴리즈
	masterUiGroups_.ForEachValue([](UIGroup* _group)
	{
		_group->Unload();
	});

	// 관련 캐쉬, 팩 모두 언로드
	loadedUiTexture_.ForEachValue([pPackManager](SgaResourceIndex& _resourceIndex)
	{
		pPackManager->releaseFrameTexture(_resourceIndex);
		pPackManager->unloadPackData(_resourceIndex.Un.SgaIndex);
	});
	loadedUiTexture_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::onUpdate(float _dt)
{
	callUIElementsUpdateCallback(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::callUIElementsUpdateCallback(float _dt)
{
	uiElementsUpdateEvent_.ForEach([&_dt](Pair<UIElement*, SGEventList<UIElement*, float>>& _elementEventPair)
	{
		_elementEventPair.value_.Invoke((UIElement*)_elementEventPair.key_, (float)_dt);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::draginit(const DragState& _state)
{
	dragState_ = _state;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::dragEnter(const SGEventMouse* _pMouseEvent)
{
	UIElement* pDragElement = dragState_.TargetElement;
	const Vec2 dragDelta = _pMouseEvent->getStartCursorPos() - dragState_.StartCursorPosition;

	pDragElement->setPosition(dragState_.StartElementPosition + dragDelta);
	pDragElement->GetRootGroup()->OnDragEnter(pDragElement, dragState_);

	dragState_.DragDelta = dragDelta;
	dragState_.Dragging = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::dragMove(const SGEventMouse* _pMouseEvent)
{
	UIElement* pDragElement = dragState_.TargetElement;
	const Vec2 dragDelta = _pMouseEvent->getStartCursorPos() - dragState_.StartCursorPosition;

	pDragElement->setPosition(dragState_.StartElementPosition + dragDelta);
	pDragElement->GetRootGroup()->OnDragMove(pDragElement, dragState_);

	dragState_.DragDelta = dragDelta;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::dragEnd()
{
	dragState_.HostElement = nullptr;
	dragState_.TargetElement = nullptr;
	dragState_.Dragging = false;
	dragState_.DragDelta = {};
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroup* UIManager::getMasterGroup(int _groupCode)
{
	if (!masterUiGroups_.Exist(_groupCode))
	{
		_LogWarn_("%d 마스터 UI 그룹이 존재하지 않습니다.", _groupCode);
		return nullptr;
	}

	return masterUiGroups_[_groupCode];
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElement* UIManager::getElement(int _elementCode)
{
	if (!uiElements_.Exist(_elementCode))
	{
		return nullptr;
	}

	return uiElements_[_elementCode];
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* UIManager::createUITexture(int _sga, int _img, int _frame, bool _linearDodge /* = false  */)
{
	ImagePack* pPack = Core::Contents.PackManager->getPackUnsafe(_sga);

	if (pPack == nullptr)
	{
		Core::Contents.PackManager->logTexture("UIManager::createUITexture()", SgaResourceIndex{ _sga, _img, _frame },
		                                       LoggerAbstract::eWarn);
		return Core::Contents.Global->getDefaultFrameTexture();
	}

	FrameTexture* pTexture = pPack->createFrameTexture(_img, _frame, _linearDodge);
	registerUITexture({ _sga, _img, _frame });
	return pTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* UIManager::createUITextureRetained(int _sga, int _img, int _frame, bool _linearDodge)
{
	FrameTexture* pTexture = createUITexture(_sga, _img, _frame, _linearDodge);
	pTexture->retain();
	return pTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::initPublic()
{
	Login = (UI_Login*)getMasterGroup(GROUP_UI_LOGIN);
	Inventory = (UI_Inventory*)getMasterGroup(GROUP_UI_INVENTORY);
	Popup = nullptr; // 팝업 매니저에서 관리
	Test = (UI_Test*)getMasterGroup(GROUP_UI_TEST);
	ChannelSelect = (UI_ChannelSelect*)getMasterGroup(GROUP_UI_CHANNEL);
}
