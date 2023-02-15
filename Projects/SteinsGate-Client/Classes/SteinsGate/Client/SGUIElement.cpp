/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 6:35:08 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUIButton.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>

USING_NS_CC;
USING_NS_JC;

SGUIElement::SGUIElement(SGUIGroup* parent)
	: m_fnMouseClickCallback(nullptr)
	, m_pParent(parent)
	, m_eState(eNormal)
	, m_bLoaded(false)
{}

void SGUIElement::load() {
	m_bLoaded = true;
}

void SGUIElement::unload() {
	m_bLoaded = false;
}

bool SGUIElement::loaded() {
	return m_bLoaded;
}

void SGUIElement::restoreState(State state) {
}

bool SGUIElement::onMouseMove(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled ||
		m_eState == ePressed)
		return true;

	Vec2 mousePos = mouseEvent->getCursorPos();
	SGRect worldRect = getWorldBoundingBox();
	bool bContainedMouse = worldRect.containsPoint(mousePos);

	if (!bContainedMouse) {
		m_eState = eNormal;
		return true;
	}

	m_eState = eOver;
	return true;
}

bool SGUIElement::onMouseDown(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled ||
		m_eState == ePressed)
		return true;

	Vec2 mousePos = mouseEvent->getCursorPos();
	bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	if (!bContainedMouse) {
		return true;
	}

	m_eState = ePressed;
	return true;
}

bool SGUIElement::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_eState != ePressed)
		return true;

	Vec2 mousePos = mouseEvent->getCursorPos();
	bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	m_eState = eNormal;

	if (!bContainedMouse) {
		return true;
	}

	if (m_fnMouseClickCallback)
		m_fnMouseClickCallback(mouseEvent);

	return true;
}

bool SGUIElement::onMouseScroll(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled)
		return true;

	return true;
}

SGRect SGUIElement::getWorldBoundingBox() {
	SGNode* parent = getParent();
	DebugAssertMsg(parent != nullptr, "addChild 되어있지 않습니다.");
	SGVec2 origin = parent->convertToWorldSpace(getPosition());
	return { origin, getContentSize() };
}

void SGUIElement::setEnabled(bool enabled) {
	m_eState = enabled ? eNormal : eDisabled;
}

