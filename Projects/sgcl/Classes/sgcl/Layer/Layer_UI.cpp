/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:23 PM
 * =====================
 *
 */

#include "Layer_UI.h"

#include "sgcl/Game/UI/UIStatic.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "sgcl/SteinsGateApp.h"
#include "sgcl/Core/AppConfig_Props.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UILayer::UILayer()
: pPrevOverStateGroup_(nullptr)
, pPrevPressedStateGroup_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UILayer* UILayer::Create()
{
	UILayer* pLayer = dbg_new UILayer;

	if (pLayer != nullptr && pLayer->init())
	{
		pLayer->autorelease();
		return pLayer;
	}

	JC_DELETE_SAFE(pLayer);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UILayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	_contentSize = g_cApp.GetDesignResolutionSize();

	UIThemeManager::Get()->SetOnThemeRefreshed([this]()
	{
		for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
		{
			UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));
			pUiGroup->RefreshThemeVisualsRecursive();
		}
	});

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::OnMouseMove(cc::EventMouse* _pMouseEvent)
{
	const Vec2 absolutePosition = _pMouseEvent->getCursorPos();
	UIGroup* pTopGroup = nullptr;

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIGroup* pUiGroup = static_cast<UIGroup*>(_children.at(i));

		if (!pUiGroup->GetUIRect().containsPoint(absolutePosition))
		{
			continue;
		}

		pTopGroup = pUiGroup;
		pTopGroup->OnMouseMoveInternal(_pMouseEvent);

		if (pTopGroup->IsRootGroup())
		{
			static_cast<UIRootGroup*>(pTopGroup)->OnMouseMove(_pMouseEvent);
		}
	}

	// 다른 그룹으로 전환된경우 마우스 Over 상태인 녀석들을 원래대로 돌려놓기 위해 벗어난 위치에서 이벤트를 한번 전파해주도록 한다.
	if (pTopGroup != pPrevOverStateGroup_ && pPrevOverStateGroup_ != nullptr)
	{
		// pPrevOverStateGroup_->restoreState(SGUIElement::eOver);
		pPrevOverStateGroup_->OnMouseMoveInternal(_pMouseEvent);

		if (pPrevOverStateGroup_->IsRootGroup())
		{
			static_cast<UIRootGroup*>(pPrevOverStateGroup_)->OnMouseMove(_pMouseEvent);
		}
	}

	pPrevOverStateGroup_ = pTopGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::OnMouseDown(cc::EventMouse* _pMouseEvent)
{
	const Vec2 absolutePosition = _pMouseEvent->getCursorPos();
	UIGroup* pTopGroup = nullptr;

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIGroup* pUiGroup = static_cast<UIGroup*>(_children.at(i));

		if (!pUiGroup->GetUIRect().containsPoint(absolutePosition))
		{
			continue;
		}

		const bool shouldPropagate = pUiGroup->OnMouseDownInternal(_pMouseEvent);
		pTopGroup = pUiGroup;

		if (pTopGroup->IsRootGroup())
		{
			static_cast<UIRootGroup*>(pTopGroup)->OnMouseDown(_pMouseEvent);
		}

		if (!shouldPropagate)
		{
			break;
		}
	}

	pPrevPressedStateGroup_ = pTopGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::OnMouseUp(cc::EventMouse* _pMouseEvent)
{
	const Vec2 absolutePosition = _pMouseEvent->getCursorPos();
	UIGroup* pTopGroup = nullptr;

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIGroup* pUiGroup = static_cast<UIGroup*>(_children.at(i));

		if (!pUiGroup->GetUIRect().containsPoint(absolutePosition))
		{
			continue;
		}

		pTopGroup = pUiGroup;
		pTopGroup->OnMouseUpInternal(_pMouseEvent);

		if (pTopGroup->IsRootGroup())
		{
			static_cast<UIRootGroup*>(pTopGroup)->OnMouseUp(_pMouseEvent);
		}
	}

	// 다른 그룹으로 전환된경우 마우스 Pressed 상태인 녀석들을 원래대로 돌려놔야함, 벗어난 위치에서 이벤트를 한번 전파해주도록 한다.
	if (pTopGroup != pPrevPressedStateGroup_ && pPrevPressedStateGroup_ != nullptr)
	{
		// pPrevPressedStateGroup_->restoreState(SGUIElement::ePressed);
		pPrevPressedStateGroup_->OnMouseUpInternal(_pMouseEvent);

		if (pPrevPressedStateGroup_->IsRootGroup())
		{
			static_cast<UIRootGroup*>(pPrevPressedStateGroup_)->OnMouseUp(_pMouseEvent);
		}
	}

	pPrevPressedStateGroup_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::OnMouseScroll(cc::EventMouse* _pMouseEvent) const
{
	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));
		const bool shouldPropagate = pUiGroup->OnMouseScrollInternal(_pMouseEvent);

		if (pUiGroup->IsRootGroup())
		{
			static_cast<UIRootGroup*>(pUiGroup)->OnMouseDown(_pMouseEvent);
		}

		if (!shouldPropagate)
		{
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::update(float _delta)
{
	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));
		pUiGroup->ForEachRecursiveSpecificType<UIStatic>([](UIStatic* _pChild)
		{
			_pChild->SetDebugVisible(g_cAppProps.GetBool(CONF_PROPS_DRAW_UI_STATIC));
		});
		pUiGroup->OnUpdate(_delta);
	}

	g_cUIMgr.OnUpdate(_delta);
	UIThemeManager::Get()->Update(_delta);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::onKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));

		if (!pUiGroup->OnKeyPressed(_keyCode, _pEvent))
		{
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::onKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));

		if (!pUiGroup->OnKeyReleased(_keyCode, _pEvent))
		{
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroup* UILayer::FindGroup(int _groupCode)
{
	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));

		if (pUiGroup && pUiGroup->GetName() && strcmp(pUiGroup->GetName(), "") != 0)
		{
			// Find by tag stored with the group (legacy support)
			if (pUiGroup->getTag() == _groupCode)
				return pUiGroup;
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::ForEach(const jc::Action<UIRootGroup*>& _actionFn)
{
	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(_children.at(i));
		_actionFn(pUiGroup);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// AddUIGroup(int) is deprecated in the new architecture; use UIManager::Show() instead.
void UILayer::AddUIGroup(int /*_groupCode*/, int _zOrder)
{
	_LogWarn_("AddUIGroup(int) is deprecated. Use UIManager::Show() instead.");
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::AddUIGroup(UIRootGroup* _pGroup, int _zOrder)
{
	if (!_pGroup->Loaded())
	{
		_pGroup->Load();
	}

	addChild(_pGroup, _zOrder);

	if (_pGroup->IsRootGroup())
	{
		_pGroup->OnAdded();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::RemoveUIGroup(int _groupCode)
{
	Node* pChild = getChildByTag(_groupCode);

	if (pChild == nullptr)
	{
		_LogWarn_("%d 그룹을 제거하는데 실패했습니다.", _groupCode);
		return;
	}

	UIRootGroup* pUiGroup = static_cast<UIRootGroup*>(pChild);
	pUiGroup->OnRemoved();
	removeChild(pChild);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::RemoveUIGroup(UIRootGroup* _pGroup)
{
	Node* pChild = nullptr;

	for (int i = static_cast<int>(_children.size()) - 1; i >= 0; --i)
	{
		if (_children.at(i) == _pGroup)
		{
			pChild = _pGroup;
			break;
		}
	}

	if (pChild == nullptr)
	{
		_LogWarn_("%s 그룹을 제거하는데 실패했습니다.", _pGroup->GetName());
		return;
	}

	_pGroup->OnRemoved();
	removeChild(_pGroup);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::Clear()
{
	jc::Vector<UIRootGroup*> tempList; // 레퍼런스 카운트가 1일 경우 레이어에서 해제될때 카운트가 0이되서 소멸해버림.

	ForEach([&tempList](UIRootGroup* child)
	{
		child->retain();
		tempList.PushBack(child);
	});

	removeAllChildren();

	for (int i = 0; i < tempList.Size(); ++i)
	{
		tempList[i]->OnRemoved();
		tempList[i]->release();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UILayer::ClearUnload()
{
	Clear();
	g_cUIMgr.UnloadAll();
}
