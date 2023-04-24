/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:23 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UILayer.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/UIStatic.h>
#include <SteinsGate/Client/UIManager.h>
#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

UILayer::UILayer()
	: m_pPrevOverStateGroup(nullptr)
	, m_pPrevPressedStateGroup(nullptr)
{}

UILayer* UILayer::create() {
	UILayer* pUILayer = dbg_new UILayer;

	if (pUILayer && pUILayer->init()) {
		pUILayer->autorelease();
		return pUILayer;
	}

	DeleteSafe(pUILayer);
	return nullptr;
}

bool UILayer::init() {
	if (!Layer::init())
		return false;

	setContentSize(CoreClient_v->ResolutionWidth, CoreClient_v->ResolutionHeight);
	return true;
}

void UILayer::onMouseMove(SGEventMouse* mouseEvent) {

	UIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIGroup*>(_children.at(i));
		if (uiGroup->getBoundingBox().containsPoint(mouseEvent->getCursorPos())) {
			pTopGroup = uiGroup;
			pTopGroup->onMouseMove(mouseEvent);
			break;
		}
	}

	// 다른 그룹으로 전환된경우 마우스 Over 상태인 녀석들을 원래대로 돌려놓기 위해 벗어난 위치에서 이벤트를 한번 전파해주도록 한다.
	if (pTopGroup != m_pPrevOverStateGroup && m_pPrevOverStateGroup != nullptr) {
		// m_pPrevOverStateGroup->restoreState(SGUIElement::eOver);
		m_pPrevOverStateGroup->onMouseMove(mouseEvent);
	}

	m_pPrevOverStateGroup = pTopGroup;
}

void UILayer::onMouseDown(SGEventMouse* mouseEvent) {
	UIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIGroup*>(_children.at(i));
		if (uiGroup->getBoundingBox().containsPoint(mouseEvent->getCursorPos()) &&
			uiGroup->onMouseDown(mouseEvent) == false) {
			pTopGroup = uiGroup;
			break;
		}
	}

	m_pPrevPressedStateGroup = pTopGroup;
}

void UILayer::onMouseUp(SGEventMouse* mouseEvent) {
	UIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIGroup*>(_children.at(i));
		if (uiGroup->getBoundingBox().containsPoint(mouseEvent->getCursorPos())) {
			pTopGroup = uiGroup;
			pTopGroup->onMouseUp(mouseEvent);
			break;
		}
	}

	// 다른 그룹으로 전환된경우 마우스 Pressed 상태인 녀석들을 원래대로 돌려놔야함, 벗어난 위치에서 이벤트를 한번 전파해주도록 한다.
	if (pTopGroup != m_pPrevPressedStateGroup && m_pPrevPressedStateGroup != nullptr) {
		// m_pPrevPressedStateGroup->restoreState(SGUIElement::ePressed);
		m_pPrevPressedStateGroup->onMouseUp(mouseEvent);
	}

	m_pPrevPressedStateGroup = nullptr;
}

void UILayer::onMouseScroll(SGEventMouse* mouseEvent) const {
	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		if (!uiGroup->onMouseScroll(mouseEvent)) {
			return;
		}
	}
}

void UILayer::update(float delta) {

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		uiGroup->forEachRecursiveSpecificType<UIStatic>([](auto child) { child->setDebugVisible(CoreGlobal_v->DrawUIStatic); });

		if (!uiGroup->onUpdate(delta)) {
			return;
		}
	}

	CoreUIManager_v->onUpdate(delta);
}

void UILayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		if (!uiGroup->onKeyPressed(keyCode, event)) {
			return;
		}
	}
}

void UILayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		if (!uiGroup->onKeyReleased(keyCode, event)) {
			return;
		}
	}
}

UIMasterGroup* UILayer::findGroup(int groupCode) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		if (uiGroup->getCode() == groupCode) {
			return uiGroup;
		}
	}

	return nullptr;
}

void UILayer::forEach(const SGActionFn<UIMasterGroup*>& actionFn) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		actionFn(uiGroup);
	}
}

void UILayer::addUIGroup(int groupCode) {
	UIMasterGroup* pGroup = CoreUIManager_v->getMasterGroup(groupCode);

	if (!pGroup->loaded())
		pGroup->load();

	addChild(pGroup, 0, pGroup->getCode());
	pGroup->onAdded();
}

void UILayer::removeUIGroup(int groupCode) {
	Node* pChild = getChildByTag(groupCode);
	if (pChild == nullptr) {
		_LogWarn_("%d 그룹을 제거하는데 실패했습니다.", groupCode);
		return;
	}

	UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(pChild);
	removeChild(pChild);
	uiGroup->onRemoved();
}

void UILayer::clear() {
	forEach([](UIMasterGroup* child) {child->onRemoved(); });
	removeAllChildren();
	
}

void UILayer::clearUnload() {
	forEach([](UIMasterGroup* child) {child->onRemoved(); });
	removeAllChildren();

	CoreUIManager_v->unloadAll();
}


