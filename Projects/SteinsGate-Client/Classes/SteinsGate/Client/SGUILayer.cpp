/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:23 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUILayer.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGUIStatic.h>
#include <SteinsGate/Client/SGUIManager.h>
#include <SteinsGate/Client/SGUIMasterGroup.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

SGUILayer::SGUILayer()
	: m_pPrevOverStateGroup(nullptr)
	, m_pPrevPressedStateGroup(nullptr)
{}

SGUILayer* SGUILayer::create() {
	SGUILayer* pUILayer = dbg_new SGUILayer;

	if (pUILayer && pUILayer->init()) {
		pUILayer->autorelease();
		return pUILayer;
	}

	DeleteSafe(pUILayer);
	return nullptr;
}

bool SGUILayer::init() {
	if (!Layer::init())
		return false;

	setContentSize(CoreClient_v->ResolutionWidth, CoreClient_v->ResolutionHeight);
	return true;
}

void SGUILayer::onMouseMove(SGEventMouse* mouseEvent) {

	SGUIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
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

void SGUILayer::onMouseDown(SGEventMouse* mouseEvent) {
	SGUIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
		if (uiGroup->getBoundingBox().containsPoint(mouseEvent->getCursorPos()) &&
			uiGroup->onMouseDown(mouseEvent) == false) {
			pTopGroup = uiGroup;
			break;
		}
	}

	m_pPrevPressedStateGroup = pTopGroup;
}

void SGUILayer::onMouseUp(SGEventMouse* mouseEvent) {
	SGUIGroup* pTopGroup = nullptr;

	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
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

void SGUILayer::onMouseScroll(SGEventMouse* mouseEvent) const {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIMasterGroup*>(_children.at(i));
		if (!uiGroup->onMouseScroll(mouseEvent)) {
			return;
		}
	}
}

void SGUILayer::update(float delta) {

	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIMasterGroup* uiGroup = static_cast<SGUIMasterGroup*>(_children.at(i));
		uiGroup->forEachRecursiveSpecificType<SGUIStatic>([](auto child) { child->setDebugVisible(CoreGlobal_v->DrawUIStatic); });

		if (!uiGroup->onUpdate(delta)) {
			return;
		}
	}

	CoreUIManager_v->onUpdate(delta);
}

void SGUILayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIMasterGroup* uiGroup = static_cast<SGUIMasterGroup*>(_children.at(i));
		if (!uiGroup->onKeyPressed(keyCode, event)) {
			return;
		}
	}
}

void SGUILayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIMasterGroup* uiGroup = static_cast<SGUIMasterGroup*>(_children.at(i));
		if (!uiGroup->onKeyReleased(keyCode, event)) {
			return;
		}
	}
}

SGUIMasterGroup* SGUILayer::findGroup(int groupCode) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIMasterGroup* uiGroup = static_cast<SGUIMasterGroup*>(_children.at(i));
		if (uiGroup->getCode() == groupCode) {
			return uiGroup;
		}
	}

	return nullptr;
}

void SGUILayer::forEach(const SGActionFn<SGUIMasterGroup*>& actionFn) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIMasterGroup* uiGroup = static_cast<SGUIMasterGroup*>(_children.at(i));
		actionFn(uiGroup);
	}
}

void SGUILayer::addUIGroup(int groupCode) {
	SGUIMasterGroup* pGroup = CoreUIManager_v->getMasterGroup(groupCode);

	if (!pGroup->loaded())
		pGroup->load();

	addChild(pGroup, 0, pGroup->getCode());
	pGroup->onAdded();
}

void SGUILayer::removeUIGroup(int groupCode) {
	Node* pChild = getChildByTag(groupCode);
	if (pChild == nullptr) {
		_LogWarn_("%d 그룹을 제거하는데 실패했습니다.", groupCode);
		return;
	}

	SGUIMasterGroup* uiGroup = static_cast<SGUIMasterGroup*>(pChild);
	removeChild(pChild);
	uiGroup->onRemoved();
}

void SGUILayer::clear() {
	forEach([](SGUIMasterGroup* child) {child->onRemoved(); });
	removeAllChildren();
	
}

void SGUILayer::clearUnload() {
	forEach([](SGUIMasterGroup* child) {child->onRemoved(); });
	removeAllChildren();

	CoreUIManager_v->unloadAll();
}


