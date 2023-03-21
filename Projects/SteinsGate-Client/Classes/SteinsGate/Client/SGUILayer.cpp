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

	setContentSize(CoreInfo_v->ResolutionWidth, CoreInfo_v->ResolutionHeight);

	


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

	// 다른 그룹으로 전환된경우 마우스 Over 상태인 녀석들을 원래대로 돌려놔야함
	if (pTopGroup != m_pPrevOverStateGroup && m_pPrevOverStateGroup != nullptr) {
		m_pPrevOverStateGroup->restoreState(SGUIElement::eOver);
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

	// 다른 그룹으로 전환된경우 마우스 Pressed 상태인 녀석들을 원래대로 돌려놔야함
	if (pTopGroup != m_pPrevPressedStateGroup && m_pPrevPressedStateGroup != nullptr) {
		m_pPrevPressedStateGroup->restoreState(SGUIElement::ePressed);
	}

	m_pPrevPressedStateGroup = nullptr;
}

void SGUILayer::onMouseScroll(SGEventMouse* mouseEvent) const {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
		if (!uiGroup->onMouseScroll(mouseEvent)) {
			return;
		}
	}
}

void SGUILayer::update(float delta) {

	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
		uiGroup->forEachRecursiveSpecificType<SGUIStatic>([](auto child) { child->setDebugVisible(CoreGlobal_v->DrawUIStatic); });

		if (!uiGroup->onUpdate(delta)) {
			return;
		}
	}
}

void SGUILayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
		if (!uiGroup->onKeyPressed(keyCode, event)) {
			return;
		}
	}
}

void SGUILayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	for (int i = _children.size() - 1; i >= 0; i--) {
		SGUIGroup* uiGroup = static_cast<SGUIGroup*>(_children.at(i));
		if (!uiGroup->onKeyReleased(keyCode, event)) {
			return;
		}
	}
}

void SGUILayer::addUIGroup(int groupCode) {
	SGUIGroup* pGroup = SGUIManager::get()->getGroup(groupCode);

	if (!pGroup->loaded())
		pGroup->load();

	addChild(pGroup, 0, pGroup->getCode());
}

void SGUILayer::removeUIGroup(int groupCode) {
	removeChildByTag(groupCode);
}

void SGUILayer::clear() {
	removeAllChildren();
}

void SGUILayer::clearUnload() {
	if (_children.size() > 0)
		removeAllChildren();

	CoreUIManager_v->unloadAll();
}


