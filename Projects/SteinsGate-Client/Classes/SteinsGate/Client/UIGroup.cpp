/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:49:38 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIGroup.h"

#include <SteinsGate/Client/UIRootGroup.h>
#include <SteinsGate/Client/UIButton.h>
#include <SteinsGate/Client/UILabel.h>
#include <SteinsGate/Client/UISprite.h>
#include <SteinsGate/Client/UIEditBox.h>
#include <SteinsGate/Client/UICheckBox.h>
#include <SteinsGate/Client/UIToggleButton.h>
#include <SteinsGate/Client/UIScrollBar.h>
#include <SteinsGate/Client/UIProgressBar.h>
#include <SteinsGate/Client/UIStatic.h>

USING_NS_CC;
USING_NS_JC;

#define SG_CURSOR_POSITION_GUARD(mouse_event, cursor_pos) UIGroup::CursorPositionGuard JCORE_CONCAT_COUNTER(__guard__)(mouse_event, cursor_pos)

UIGroup::UIGroup(UIRootGroup* _pMaster, UIGroup* _pParent)
: UIElement(_pMaster, _pParent)
, groupInfo_(nullptr)
{
}

UIGroup::UIGroup(UIRootGroup* _pMaster, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner)
: UIElement(_pMaster, _pParent, _pGroupInfo, _infoOwner)
, groupInfo_(_pGroupInfo)
{
}

UIGroup::~UIGroup()
{
}

UIGroup* UIGroup::create(UIRootGroup* _pMaster, UIGroup* _pParent)
{
	UIGroup* pGroup = dbg_new UIGroup(_pMaster, _pParent);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

UIGroup* UIGroup::create(UIRootGroup* _pMaster, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner)
{
	UIGroup* pGroup = dbg_new UIGroup(_pMaster, _pParent, _pGroupInfo, _infoOwner);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

bool UIGroup::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (groupInfo_ != nullptr)
	{
		SetInitialUISize(groupInfo_->Size);
	}

	return isInitialized_ = true;
}

void UIGroup::initChildren()
{
	if (groupInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	for (int i = 0; i < groupInfo_->InfoList.Size(); ++i)
	{
		UIGroupElemInfo* pElemInfo = &groupInfo_->InfoList[i];
		addUIElement(pElemInfo);
	}

	initChildrenPosition();
}

void UIGroup::initChildrenPosition()
{
	if (groupInfo_ == nullptr)
	{
		return;
	}

	for (int i = 0; i < groupInfo_->InfoList.Size(); ++i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));
		const UIGroupElemInfo& elemInfo = groupInfo_->InfoList[i];

		pElement->SetRelativePosition(
			elemInfo.Pos.x * Core::ClientInfo->uiScaleXFactor_,
			elemInfo.Pos.y * Core::ClientInfo->uiScaleYFactor_);

		if (pElement->IsGroup())
		{
			static_cast<UIGroup*>(pElement)->initChildrenPosition();
		}
	}
}

void UIGroup::Load()
{
	if (isLoaded_)
	{
		return;
	}

	forEachRecursive([](UIElement* _pElement)
	{
		_pElement->Load();
	});

	isLoaded_ = true;

	if (IsRootGroup())
	{
		pRootGroup_->OnLoaded();
	}
}

void UIGroup::Unload()
{
	static SGVector<UIElement*> DeveloperCreatedList;

	if (!isLoaded_)
	{
		return;
	}

	DeveloperCreatedList.Clear();

	forEachRecursive([](UIElement* _pElement)
	{
		if (_pElement->IsDeveloperCreated())
		{
			DeveloperCreatedList.PushBack(_pElement);
		}

		_pElement->Unload();
	});

	for (int i = 0; i < DeveloperCreatedList.Size(); ++i)
	{
		removeChild(DeveloperCreatedList[i]);
	}

	isLoaded_ = false;

	if (IsRootGroup())
	{
		pRootGroup_->OnUnloaded();
	}
}

void UIGroup::addChild(UIElement* _pChild)
{
	Node::addChild(_pChild);
}

bool UIGroup::OnMouseDownInternal(SGEventMouse* _pMouseEvent)
{
	const SGVec2 mousePos = _pMouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	_pMouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(_pMouseEvent, mousePos);

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));
		if (!pElement->OnMouseDownInternal(_pMouseEvent))
		{
			return false;
		}
	}

	return UIElement::OnMouseDownInternal(_pMouseEvent);
}

bool UIGroup::OnMouseMoveInternal(SGEventMouse* _pMouseEvent)
{
	const SGVec2 mousePos = _pMouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	_pMouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(_pMouseEvent, mousePos);

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));

		if (!pElement->OnMouseMoveInternal(_pMouseEvent))
		{
			return false;
		}
	}

	return UIElement::OnMouseMoveInternal(_pMouseEvent);
}

bool UIGroup::OnMouseUpInternal(SGEventMouse* _pMouseEvent)
{
	const SGVec2 mousePos = _pMouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	_pMouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(_pMouseEvent, mousePos);

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));
		if (!pElement->OnMouseUpInternal(_pMouseEvent))
		{
			return false;
		}
	}

	return UIElement::OnMouseUpInternal(_pMouseEvent);
}

bool UIGroup::OnMouseScrollInternal(SGEventMouse* _pMouseEvent)
{
	const SGVec2 mousePos = _pMouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	_pMouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(_pMouseEvent, mousePos);

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));
		if (!pElement->OnMouseScrollInternal(_pMouseEvent))
		{
			return false;
		}
	}

	return UIElement::OnMouseScrollInternal(_pMouseEvent);
}

UIElement* UIGroup::getAt(int _index)
{
	if (_index >= _children.size())
	{
		_LogWarn_("%d 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", pBaseInfo_->code_, _index);
		return nullptr;
	}

	return static_cast<UIElement*>(_children.at(_index));
}

UIElement* UIGroup::findElement(int _code)
{
	UIElement* pElement = findElementRecursiveInternal(this, _code);

	if (pElement == nullptr)
	{
		_LogWarn_("%d를 찾지 못했습니다.", _code);
	}

	return pElement;
}

UIGroup* UIGroup::findGroup(int _groupCode)
{
	return findElementTemplated<UIGroup>(this, _groupCode);
}

UIButton* UIGroup::findButton(int _buttonCode)
{
	return findElementTemplated<UIButton>(this, _buttonCode);
}

UISprite* UIGroup::findSprite(int _spriteCode)
{
	return findElementTemplated<UISprite>(this, _spriteCode);
}

UILabel* UIGroup::findLabel(int _labelCode)
{
	return findElementTemplated<UILabel>(this, _labelCode);
}

UICheckBox* UIGroup::findCheckBox(int _checkBoxCode)
{
	return findElementTemplated<UICheckBox>(this, _checkBoxCode);
}

UIEditBox* UIGroup::findEditBox(int _editBoxCode)
{
	return findElementTemplated<UIEditBox>(this, _editBoxCode);
}

UIToggleButton* UIGroup::findToggleButton(int _toggleButtonCode)
{
	return findElementTemplated<UIToggleButton>(this, _toggleButtonCode);
}

UIProgressBar* UIGroup::findProgressBar(int _progressBarCode)
{
	return findElementTemplated<UIProgressBar>(this, _progressBarCode);
}

UIScrollBar* UIGroup::findScrollBar(int _scrollBarCode)
{
	return findElementTemplated<UIScrollBar>(this, _scrollBarCode);
}

UIStatic* UIGroup::findStatic(int _staticCode)
{
	return findElementTemplated<UIStatic>(this, _staticCode);
}

void UIGroup::addUIElement(UIGroupElemInfo* _pGroupElemInfo)
{
	UIElementInfo* pElementInfo = Core::DataManager->getUIElementInfo(_pGroupElemInfo->Code);
	UIElement* pChildElement = nullptr;

	switch (pElementInfo->Type)
	{
	case UIElementType::Group:
		pChildElement = UIGroup::create(pRootGroup_, this, static_cast<UIGroupInfo*>(pElementInfo), false);
		break;
	case UIElementType::Button:
		pChildElement = UIButton::create(pRootGroup_, this, static_cast<UIButtonInfo*>(pElementInfo), false);
		break;
	case UIElementType::Label:
		pChildElement = UILabel::create(pRootGroup_, this, static_cast<UILabelInfo*>(pElementInfo), false);
		break;
	case UIElementType::Sprite:
		pChildElement = UISprite::create(pRootGroup_, this, static_cast<UISpriteInfo*>(pElementInfo), false);
		break;
	case UIElementType::EditBox:
		pChildElement = UIEditBox::create(pRootGroup_, this, static_cast<UIEditBoxInfo*>(pElementInfo), false);
		break;
	case UIElementType::CheckBox:
		pChildElement = UICheckBox::create(pRootGroup_, this, static_cast<UICheckBoxInfo*>(pElementInfo), false);
		break;
	case UIElementType::ToggleButton:
		pChildElement = UIToggleButton::create(pRootGroup_, this, static_cast<UIToggleButtonInfo*>(pElementInfo),
		                                       false);
		break;
	case UIElementType::ProgressBar:
		pChildElement = UIProgressBar::create(pRootGroup_, this, static_cast<UIProgressBarInfo*>(pElementInfo),
		                                      false);
		break;
	case UIElementType::ScrollBar:
		pChildElement = UIScrollBar::create(pRootGroup_, this, static_cast<UIScrollBarInfo*>(pElementInfo), false);
		break;
	case UIElementType::Static:
		pChildElement = UIStatic::create(pRootGroup_, this, static_cast<UIStaticInfo*>(pElementInfo), false);
		break;
	default:
		_LogWarn_("알 수 없는 타입의 엘리먼트를 추가할려고했습니다. (%d)", pElementInfo->Type);
		return;
	}

	if (pChildElement == nullptr)
	{
		_LogError_("해당하는 UI 엘리먼트 타입의 자식을 생성하지 못했습니다.");
		return;
	}

	if (pChildElement->IsGroup())
	{
		static_cast<UIGroup*>(pChildElement)->initChildren();
	}

	addChild(pChildElement);
}

void UIGroup::forEachRecursive(const SGActionFn<UIElement*>& _action) const
{
	for (int i = 0; i < _children.size(); ++i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));

		if (pElement->GetElementType() == UIElementType::Group)
		{
			UIGroup* pGroupElement = static_cast<UIGroup*>(pElement);
			_action(pGroupElement);
			pGroupElement->forEachRecursive(_action);
			continue;
		}

		_action(pElement);
	}
}

void UIGroup::forEachRecursiveContainedSelf(const SGActionFn<UIElement*>& _action) const
{
	_action(const_cast<UIGroup*>(this));
	forEachRecursive(_action);
}

void UIGroup::forEach(const SGActionFn<UIElement*>& _action) const
{
	for (int i = 0; i < _children.size(); ++i)
	{
		_action(static_cast<UIElement*>(_children.at(i)));
	}
}

void UIGroup::forEachContainedSelf(const SGActionFn<UIElement*>& _action) const
{
	_action(const_cast<UIGroup*>(this));
	forEach(_action);
}

void UIGroup::RestoreState(State _state)
{
	if (state_ == eDisabled)
	{
		return;
	}

	forEachRecursive([_state](UIElement* _pChild)
	{
		_pChild->RestoreState(_state);
	});
}

void UIGroup::SetUISize(const SGSize& _size)
{
	if (!isResizable_)
	{
		return;
	}

	Vec2 relativePos = GetRelativePosition();
	uiSize_ = _size;

	const float scaleX = getScaleX();
	const float scaleY = getScaleY();

	forEach([&](UIElement* _pChild)
	{
		if (!_pChild->IsResizable())
		{
			return;
		}

		_pChild->SetUIScale(scaleX, scaleY);
	});

	SetRelativePosition(relativePos);
}

void UIGroup::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::Group)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(groupInfo_);
	}

	pBaseInfo_ = _pInfo;
	groupInfo_ = static_cast<UIGroupInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIGroup::setInfoGroup(UIGroupInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

UIElement* UIGroup::findElementRecursiveInternal(UIGroup* _pParent, int _code)
{
	for (int i = 0; i < _pParent->_children.size(); ++i)
	{
		UIElement* pElement = static_cast<UIElement*>(_pParent->_children.at(i));

		if (pElement->GetCode() == _code)
		{
			return pElement;
		}

		if (pElement->GetElementType() == UIElementType::Group)
		{
			UIElement* pFound = findElementRecursiveInternal(static_cast<UIGroup*>(pElement), _code);
			if (pFound != nullptr)
			{
				return pFound;
			}
		}
	}

	return nullptr;
}
