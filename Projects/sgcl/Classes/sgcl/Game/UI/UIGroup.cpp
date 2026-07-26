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
#include "sgcl/Game/UI/UIXmlLoader.h"

USING_NS_CC;
USING_NS_JC;

#define SG_CURSOR_POSITION_GUARD(mouse_event, cursor_pos) UIGroup::CursorPositionGuard JC_CONCAT_COUNTER(__guard__)(mouse_event, cursor_pos)

UIGroup::UIGroup(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, groupInfo_(nullptr)
{
}

UIGroup::UIGroup(UIRootGroup* _pRoot, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pGroupInfo, _infoOwner)
, groupInfo_(_pGroupInfo)
{
}

UIGroup::~UIGroup()
{
}

UIGroup* UIGroup::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UIGroup* pGroup = dbg_new UIGroup(_pRoot, _pParent);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

UIGroup* UIGroup::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner)
{
	UIGroup* pGroup = dbg_new UIGroup(_pRoot, _pParent, _pGroupInfo, _infoOwner);
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
		SetInitialUISize(groupInfo_->size_);
	}

	return isInitialized_ = true;
}

static UIElement* CreateUIElementFromInfo(UIRootGroup* _pRoot, UIGroup* _pParent, UIElementInfo* _pInfo)
{
	switch (_pInfo->type_)
	{
	case UIElementType::Button:       return UIButton::Create(_pRoot, _pParent, static_cast<UIButtonInfo*>(_pInfo), false);
	case UIElementType::Label:        return UILabel::create(_pRoot, _pParent, static_cast<UILabelInfo*>(_pInfo), false);
	case UIElementType::Sprite:       return UISprite::Create(_pRoot, _pParent, static_cast<UISpriteInfo*>(_pInfo), false);
	case UIElementType::EditBox:      return UIEditBox::Create(_pRoot, _pParent, static_cast<UIEditBoxInfo*>(_pInfo), false);
	case UIElementType::CheckBox:     return UICheckBox::Create(_pRoot, _pParent, static_cast<UICheckBoxInfo*>(_pInfo), false);
	case UIElementType::ToggleButton: return UIToggleButton::Create(_pRoot, _pParent, static_cast<UIToggleButtonInfo*>(_pInfo), false);
	case UIElementType::ProgressBar:  return UIProgressBar::Create(_pRoot, _pParent, static_cast<UIProgressBarInfo*>(_pInfo), false);
	case UIElementType::ScrollBar:    return UIScrollBar::Create(_pRoot, _pParent, static_cast<UIScrollBarInfo*>(_pInfo), false);
	case UIElementType::Static:       return UIStatic::Create(_pRoot, _pParent, static_cast<UIStaticInfo*>(_pInfo), false);
	case UIElementType::Group:        return UIGroup::Create(_pRoot, _pParent, static_cast<UIGroupInfo*>(_pInfo), false);
	default: return nullptr;
	}
}

void UIGroup::InitFromXml()
{
	if (groupInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	for (int i = 0; i < groupInfo_->childInfoList_.Size(); ++i)
	{
		UIElementInfo* pInfo = groupInfo_->childInfoList_[i];
		UIElement* pElement = CreateUIElementFromInfo(pRootGroup_, this, pInfo);
		if (!pElement)
			continue;

		AddChild(pElement);

		if (pInfo->name_[0] != '\0')
			nameMap_[std::string(pInfo->name_)] = pElement;

		if (pInfo->type_ == UIElementType::Group)
		{
			UIGroup* pChildGroup = static_cast<UIGroup*>(pElement);
			pChildGroup->InitFromXml();
		}
	}

	InitChildrenPosition();
}

UIElement* UIGroup::FindElementByName(const char* _name)
{
	if (!_name) return nullptr;

	auto it = nameMap_.find(std::string(_name));
	if (it != nameMap_.end())
		return it->second;

	UIGroup* pGroup = dynamic_cast<UIGroup*>(this);
	if (pGroup)
	{
		for (int i = 0; i < pGroup->_children.size(); ++i)
		{
			UIElement* pElement = static_cast<UIElement*>(pGroup->_children.at(i));
			if (strcmp(pElement->GetName(), _name) == 0)
				return pElement;
			if (pElement->GetElementType() == UIElementType::Group)
			{
				UIElement* pFound = static_cast<UIGroup*>(pElement)->FindElementByName(_name);
				if (pFound)
					return pFound;
			}
		}
	}
	return nullptr;
}

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

void UIGroup::AddChild(UIElement* _pChild)
{
	Node::addChild(_pChild);
}

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

UIElement* UIGroup::GetAt(int _index)
{
	if (_index >= _children.size())
	{
		_LogWarn_("%s 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", pBaseInfo_->name_, _index);
		return nullptr;
	}

	return static_cast<UIElement*>(_children.at(_index));
}

void UIGroup::AddUIElement(UIGroupElemInfo* _pGroupElemInfo)
{
	// In the new architecture, elements are created directly from XML info
	// which is already embedded in groupInfo_->infoList_
	// This stub can be expanded if custom element creation is needed

	const char* elemName = _pGroupElemInfo->name_;

	if (elemName && elemName[0] != '\0')
	{
		nameMap_[std::string(elemName)] = nullptr; // placeholder
	}

	// Children are traversed in InitChildren()
}

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

void UIGroup::ForEachRecursiveContainedSelf(const jc::Action<UIElement*>& _action) const
{
	_action(const_cast<UIGroup*>(this));
	ForEachRecursive(_action);
}

void UIGroup::ForEach(const jc::Action<UIElement*>& _action) const
{
	for (int i = 0; i < _children.size(); ++i)
	{
		_action(static_cast<UIElement*>(_children.at(i)));
	}
}

void UIGroup::ForEachContainedSelf(const jc::Action<UIElement*>& _action) const
{
	_action(const_cast<UIGroup*>(this));
	ForEach(_action);
}

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

void UIGroup::SetInfoGroup(UIGroupInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

UIElement* UIGroup::FindElementRecursiveInternal(UIGroup* _pParent, const char* _name)
{
	if (!_name || !_pParent) return nullptr;

	for (int i = 0; i < _pParent->_children.size(); ++i)
	{
		UIElement* pElement = static_cast<UIElement*>(_pParent->_children.at(i));

		if (strcmp(pElement->GetName(), _name) == 0)
		{
			return pElement;
		}

		if (pElement->GetElementType() == UIElementType::Group)
		{
			UIElement* pFound = FindElementRecursiveInternal(static_cast<UIGroup*>(pElement), _name);
			if (pFound != nullptr)
			{
				return pFound;
			}
		}
	}

	return nullptr;
}