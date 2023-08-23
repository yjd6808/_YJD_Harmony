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

	JCORE_DELETE_SAFE(pUILayer);
	return nullptr;
}

bool UILayer::init() {
	if (!Layer::init())
		return false;

	
	_contentSize = Core::ClientInfo->GameResolutionSize;
	return true;
}

void UILayer::onMouseMove(SGEventMouse* mouseEvent) {

	const Vec2 absolutePosition = mouseEvent->getCursorPos();
	UIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIGroup*>(_children.at(i));
		if (!uiGroup->getUIRect().containsPoint(absolutePosition))
			continue;
		
		pTopGroup = uiGroup;
		pTopGroup->onMouseMoveInternal(mouseEvent);

		if (pTopGroup->isMasterGroup()) {
			static_cast<UIMasterGroup*>(pTopGroup)->onMouseMove(mouseEvent);
		}
	}

	// 다른 그룹으로 전환된경우 마우스 Over 상태인 녀석들을 원래대로 돌려놓기 위해 벗어난 위치에서 이벤트를 한번 전파해주도록 한다.
	if (pTopGroup != m_pPrevOverStateGroup && m_pPrevOverStateGroup != nullptr) {
		// m_pPrevOverStateGroup->restoreState(SGUIElement::eOver);
		m_pPrevOverStateGroup->onMouseMoveInternal(mouseEvent);

		if (m_pPrevOverStateGroup->isMasterGroup()) {
			static_cast<UIMasterGroup*>(m_pPrevOverStateGroup)->onMouseMove(mouseEvent);
		}
	}

	m_pPrevOverStateGroup = pTopGroup;
}

void UILayer::onMouseDown(SGEventMouse* mouseEvent) {
	const Vec2 absolutePosition = mouseEvent->getCursorPos();
	UIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIGroup*>(_children.at(i));
		/*if (uiGroup->getUIRect().containsPoint(absolutePosition) &&
			uiGroup->onMouseDownInternal(mouseEvent) == false) {
			pTopGroup = uiGroup;
			break;
		}*/


		if (!uiGroup->getUIRect().containsPoint(absolutePosition))
			continue;

		const bool bPropagate = uiGroup->onMouseDownInternal(mouseEvent);
		pTopGroup = uiGroup;

		if (pTopGroup->isMasterGroup()) {
			static_cast<UIMasterGroup*>(pTopGroup)->onMouseDown(mouseEvent);
		}


		if (!bPropagate) {
			break;
		}

	}

	m_pPrevPressedStateGroup = pTopGroup;
}

void UILayer::onMouseUp(SGEventMouse* mouseEvent) {
	const Vec2 absolutePosition = mouseEvent->getCursorPos();
	UIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIGroup*>(_children.at(i));
		if (!uiGroup->getUIRect().containsPoint(absolutePosition))
			continue;

		pTopGroup = uiGroup;
		pTopGroup->onMouseUpInternal(mouseEvent);

		if (pTopGroup->isMasterGroup()) {
			static_cast<UIMasterGroup*>(pTopGroup)->onMouseUp(mouseEvent);
		}
	}

	// 다른 그룹으로 전환된경우 마우스 Pressed 상태인 녀석들을 원래대로 돌려놔야함, 벗어난 위치에서 이벤트를 한번 전파해주도록 한다.
	if (pTopGroup != m_pPrevPressedStateGroup && m_pPrevPressedStateGroup != nullptr) {
		// m_pPrevPressedStateGroup->restoreState(SGUIElement::ePressed);
		m_pPrevPressedStateGroup->onMouseUpInternal(mouseEvent);

		if (m_pPrevPressedStateGroup->isMasterGroup()) {
			static_cast<UIMasterGroup*>(m_pPrevPressedStateGroup)->onMouseUp(mouseEvent);
		}
	}

	m_pPrevPressedStateGroup = nullptr;
}

void UILayer::onMouseScroll(SGEventMouse* mouseEvent) const {
	for (int i = _children.size() - 1; i >= 0; i--) {
		UIGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		const bool bPropagate = uiGroup->onMouseScrollInternal(mouseEvent);

		if (uiGroup->isMasterGroup()) {
			static_cast<UIMasterGroup*>(uiGroup)->onMouseDown(mouseEvent);
		}

		if (!bPropagate) {
			break;
		}
	}
}

void UILayer::update(float delta) {

	for (int i = _children.size() - 1; i >= 0; i--) {
		UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(_children.at(i));
		uiGroup->forEachRecursiveSpecificType<UIStatic>([](auto child) { child->setDebugVisible(Core::Contents.Global->DrawUIStatic); });
		uiGroup->onUpdate(delta);
	}

	Core::Contents.UIManager->onUpdate(delta);
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

void UILayer::addUIGroup(int groupCode, int zorder) {
	UIMasterGroup* pGroup = Core::Contents.UIManager->getMasterGroup(groupCode);

	if (pGroup == nullptr) {
		_LogWarn_("%d 마스터 UI그룹을 찾지 못했습니다. %s파일을 확인해주세요.", ConfigFileType::FileName[ConfigFileType::UI]);
		return;
	}

	if (!pGroup->loaded())
		pGroup->load();

	
	addChild(pGroup, zorder, pGroup->getCode());
	pGroup->onAdded();
	
	
}

void UILayer::addUIGroup(UIMasterGroup* group, int zorder) {
	if (!group->loaded())
		group->load();

	addChild(group, zorder);

	if (group->isMasterGroup())
		group->onAdded();
}

void UILayer::removeUIGroup(int groupCode) {
	Node* pChild = getChildByTag(groupCode);
	if (pChild == nullptr) {
		_LogWarn_("%d 그룹을 제거하는데 실패했습니다.", groupCode);
		return;
	}

	UIMasterGroup* uiGroup = static_cast<UIMasterGroup*>(pChild);
	uiGroup->onRemoved();
	removeChild(pChild);
	
}

void UILayer::removeUIGroup(UIMasterGroup* group) {

	Node* pChild = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		if (_children.at(i) == group) {
			pChild = group;
			break;
		}
	}

	if (pChild == nullptr) {
		_LogWarn_("%d 그룹을 제거하는데 실패했습니다.", group->getCode());
		return;
	}

	group->onRemoved();
	removeChild(group);
}

void UILayer::clear() {
	SGVector<UIMasterGroup*> vTempList;	// 레퍼런스 카운트가 1일 경우 레이어에서 해제될때 카운트가 0이되서 소멸해버림.

	forEach([&vTempList](UIMasterGroup* child) {
		child->retain();
		vTempList.PushBack(child);
	});

	removeAllChildren();

	for (int i = 0; i < vTempList.Size(); ++i) {
		vTempList[i]->onRemoved();
		vTempList[i]->release();
	}
}

void UILayer::clearUnload() {
	clear();
	Core::Contents.UIManager->unloadAll();
}


