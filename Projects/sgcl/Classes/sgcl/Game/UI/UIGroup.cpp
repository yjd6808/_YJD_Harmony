/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:49:38 PM
 * =====================
 *
 */

#include "Game/UI/UIGroup.h"

#include "sg/Util/DescLoaderMgr.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/UIButton.h"
#include "sgcl/Game/UI/UILabel.h"
#include "sgcl/Game/UI/UISprite.h"
#include "sgcl/Game/UI/UIEditBox.h"
#include "sgcl/Game/UI/UICheckBox.h"
#include "sgcl/Game/UI/UIToggleButton.h"
#include "sgcl/Game/UI/UIScrollBar.h"
#include "sgcl/Game/UI/UIProgressBar.h"
#include "sgcl/Game/UI/UIStatic.h"

USING_NS_CC;
USING_NS_JC;

#define SG_CURSOR_POSITION_GUARD(mouse_event, cursor_pos) UIGroup::CursorPositionGuard JC_CONCAT_COUNTER(__guard__)(mouse_event, cursor_pos)

//////////////////////////////////////////////////////////////////////////////////////////
UIGroup::UIGroup(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, groupInfo_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroup::UIGroup(UIRootGroup* _pRoot, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pGroupInfo, _infoOwner)
, groupInfo_(_pGroupInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroup::~UIGroup()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroup* UIGroup::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UIGroup* pGroup = dbg_new UIGroup(_pRoot, _pParent);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroup* UIGroup::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner)
{
	UIGroup* pGroup = dbg_new UIGroup(_pRoot, _pParent, _pGroupInfo, _infoOwner);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIGroup::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (groupInfo_ != nullptr)
	{
		SetInitialUISize(groupInfo_->size_);
	}

	return isInitialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::InitChildren()
{
	if (groupInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	for (int i = 0; i < groupInfo_->infoList_.Size(); ++i)
	{
		UIGroupElemInfo* pElemInfo = &groupInfo_->infoList_[i];
		AddUIElement(pElemInfo);
	}

	InitChildrenPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::InitChildrenPosition()
{
	if (groupInfo_ == nullptr)
	{
		return;
	}

	for (int i = 0; i < groupInfo_->infoList_.Size(); ++i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));
		const UIGroupElemInfo& elemInfo = groupInfo_->infoList_[i];
		Vec2 pos = elemInfo.pos_;
		ApplyUIScaleFactor(pos);

		pElement->SetRelativePosition(pos);
		if (pElement->IsGroup())
		{
			static_cast<UIGroup*>(pElement)->InitChildrenPosition();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::Load()
{
	if (isLoaded_)
	{
		return;
	}

	ForEachRecursive([](UIElement* _pElement)
	{
		_pElement->Load();
	});

	isLoaded_ = true;

	if (IsRootGroup())
	{
		pRootGroup_->OnLoaded();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::Unload()
{
	static jc::Vector<UIElement*> developerCreatedList;

	if (!isLoaded_)
	{
		return;
	}

	developerCreatedList.Clear();

	ForEachRecursive([](UIElement* _pElement)
	{
		if (_pElement->IsDeveloperCreated())
		{
			developerCreatedList.PushBack(_pElement);
		}

		_pElement->Unload();
	});

	for (int i = 0; i < developerCreatedList.Size(); ++i)
	{
		removeChild(developerCreatedList[i]);
	}

	isLoaded_ = false;

	if (IsRootGroup())
	{
		pRootGroup_->OnUnloaded();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::AddChild(UIElement* _pChild)
{
	Node::addChild(_pChild);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIGroup::OnMouseDownInternal(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 mousePos = _pMouseEvent->getCursorPos();
	const cc::vec2 relativePos = mousePos - _position;
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

//////////////////////////////////////////////////////////////////////////////////////////
bool UIGroup::OnMouseMoveInternal(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 mousePos = _pMouseEvent->getCursorPos();
	const cc::vec2 relativePos = mousePos - _position;
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

//////////////////////////////////////////////////////////////////////////////////////////
bool UIGroup::OnMouseUpInternal(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 mousePos = _pMouseEvent->getCursorPos();
	const cc::vec2 relativePos = mousePos - _position;
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

//////////////////////////////////////////////////////////////////////////////////////////
bool UIGroup::OnMouseScrollInternal(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 mousePos = _pMouseEvent->getCursorPos();
	const cc::vec2 relativePos = mousePos - _position;
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

//////////////////////////////////////////////////////////////////////////////////////////
UIElement* UIGroup::GetAt(int _index)
{
	if (_index >= _children.size())
	{
		_LogWarn_("%d 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", pBaseInfo_->code_, _index);
		return nullptr;
	}

	return static_cast<UIElement*>(_children.at(_index));
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElement* UIGroup::FindElement(int _code)
{
	UIElement* pElement = FindElementRecursiveInternal(this, _code);

	if (pElement == nullptr)
	{
		_LogWarn_("%d를 찾지 못했습니다.", _code);
	}

	return pElement;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroup* UIGroup::FindGroup(int _groupCode)
{
	return FindElementTemplated<UIGroup>(this, _groupCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIButton* UIGroup::FindButton(int _buttonCode)
{
	return FindElementTemplated<UIButton>(this, _buttonCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UISprite* UIGroup::FindSprite(int _spriteCode)
{
	return FindElementTemplated<UISprite>(this, _spriteCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UILabel* UIGroup::FindLabel(int _labelCode)
{
	return FindElementTemplated<UILabel>(this, _labelCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox* UIGroup::FindCheckBox(int _checkBoxCode)
{
	return FindElementTemplated<UICheckBox>(this, _checkBoxCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIEditBox* UIGroup::FindEditBox(int _editBoxCode)
{
	return FindElementTemplated<UIEditBox>(this, _editBoxCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIToggleButton* UIGroup::FindToggleButton(int _toggleButtonCode)
{
	return FindElementTemplated<UIToggleButton>(this, _toggleButtonCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIProgressBar* UIGroup::FindProgressBar(int _progressBarCode)
{
	return FindElementTemplated<UIProgressBar>(this, _progressBarCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIScrollBar* UIGroup::FindScrollBar(int _scrollBarCode)
{
	return FindElementTemplated<UIScrollBar>(this, _scrollBarCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic* UIGroup::FindStatic(int _staticCode)
{
	return FindElementTemplated<UIStatic>(this, _staticCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::AddUIElement(UIGroupElemInfo* _pGroupElemInfo)
{
	UIElementInfo* pElementInfo = g_cDescMgr.GetUIElementInfo(_pGroupElemInfo->code_);
	UIElement* pChildElement = nullptr;

	switch (pElementInfo->type_)
	{
	case UIElementType::Group:
		pChildElement = UIGroup::Create(pRootGroup_, this, static_cast<UIGroupInfo*>(pElementInfo), false);
		break;
	case UIElementType::Button:
		pChildElement = UIButton::Create(pRootGroup_, this, static_cast<UIButtonInfo*>(pElementInfo), false);
		break;
	case UIElementType::Label:
		pChildElement = UILabel::create(pRootGroup_, this, static_cast<UILabelInfo*>(pElementInfo), false);
		break;
	case UIElementType::Sprite:
		pChildElement = UISprite::Create(pRootGroup_, this, static_cast<UISpriteInfo*>(pElementInfo), false);
		break;
	case UIElementType::EditBox:
		pChildElement = UIEditBox::Create(pRootGroup_, this, static_cast<UIEditBoxInfo*>(pElementInfo), false);
		break;
	case UIElementType::CheckBox:
		pChildElement = UICheckBox::Create(pRootGroup_, this, static_cast<UICheckBoxInfo*>(pElementInfo), false);
		break;
	case UIElementType::ToggleButton:
		pChildElement = UIToggleButton::Create(pRootGroup_, this, static_cast<UIToggleButtonInfo*>(pElementInfo),
		                                       false);
		break;
	case UIElementType::ProgressBar:
		pChildElement = UIProgressBar::Create(pRootGroup_, this, static_cast<UIProgressBarInfo*>(pElementInfo),
		                                      false);
		break;
	case UIElementType::ScrollBar:
		pChildElement = UIScrollBar::Create(pRootGroup_, this, static_cast<UIScrollBarInfo*>(pElementInfo), false);
		break;
	case UIElementType::Static:
		pChildElement = UIStatic::Create(pRootGroup_, this, static_cast<UIStaticInfo*>(pElementInfo), false);
		break;
	default:
		_LogWarn_("알 수 없는 타입의 엘리먼트를 추가할려고했습니다. (%d)", pElementInfo->type_);
		return;
	}

	if (pChildElement == nullptr)
	{
		_LogError_("해당하는 UI 엘리먼트 타입의 자식을 생성하지 못했습니다.");
		return;
	}

	if (pChildElement->IsGroup())
	{
		static_cast<UIGroup*>(pChildElement)->InitChildren();
	}

	AddChild(pChildElement);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::ForEachRecursive(const jc::Action<UIElement*>& _action) const
{
	for (int i = 0; i < _children.size(); ++i)
	{
		UIElement* pElement = static_cast<UIElement*>(_children.at(i));

		if (pElement->GetElementType() == UIElementType::Group)
		{
			UIGroup* pGroupElement = static_cast<UIGroup*>(pElement);
			_action(pGroupElement);
			pGroupElement->ForEachRecursive(_action);
			continue;
		}

		_action(pElement);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::ForEachRecursiveContainedSelf(const jc::Action<UIElement*>& _action) const
{
	_action(const_cast<UIGroup*>(this));
	ForEachRecursive(_action);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::ForEach(const jc::Action<UIElement*>& _action) const
{
	for (int i = 0; i < _children.size(); ++i)
	{
		_action(static_cast<UIElement*>(_children.at(i)));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::ForEachContainedSelf(const jc::Action<UIElement*>& _action) const
{
	_action(const_cast<UIGroup*>(this));
	ForEach(_action);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::RestoreState(State _state)
{
	if (state_ == eDisabled)
	{
		return;
	}

	ForEachRecursive([_state](UIElement* _pChild)
	{
		_pChild->RestoreState(_state);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::SetUISize(const cc::size& _contentSize)
{
	if (!isResizable_)
	{
		return;
	}

	Vec2 relativePos = GetRelativePosition();
	uiSize_ = _contentSize;

	const float scaleX = getScaleX();
	const float scaleY = getScaleY();

	ForEach([&](UIElement* _pChild)
	{
		if (!_pChild->IsResizable())
		{
			return;
		}

		_pChild->SetUIScale(scaleX, scaleY);
	});

	SetRelativePosition(relativePos);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::Group)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JC_DELETE_SAFE(groupInfo_);
	}

	pBaseInfo_ = _pInfo;
	groupInfo_ = static_cast<UIGroupInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIGroup::SetInfoGroup(UIGroupInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElement* UIGroup::FindElementRecursiveInternal(UIGroup* _pParent, int _code)
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
			UIElement* pFound = FindElementRecursiveInternal(static_cast<UIGroup*>(pElement), _code);
			if (pFound != nullptr)
			{
				return pFound;
			}
		}
	}

	return nullptr;
}
