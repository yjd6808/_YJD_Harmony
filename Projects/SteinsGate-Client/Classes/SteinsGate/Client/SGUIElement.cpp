/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 6:35:08 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIElement.h"

USING_NS_CC;
USING_NS_JC;

SGUIElement::SGUIElement(SGUIGroup* parent, SGUIElementInfo* info)
	: m_pBaseInfo(info)
	, m_pParent(parent)
	, m_eState(eNormal)
	, m_bLoaded(false)
	, m_bFocused(false)
	, m_fnMouseClickCallback(nullptr) {
}

void SGUIElement::load() {
	m_bLoaded = true;
}

void SGUIElement::unload() {
	m_bLoaded = false;
}

bool SGUIElement::loaded() const {
	return m_bLoaded;
}

void SGUIElement::focus() {
	m_bFocused = true;
}

void SGUIElement::unfocus() {
	m_bFocused = false;
}

void SGUIElement::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	m_eState = eNormal;
}

bool SGUIElement::onMouseMove(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled ||
		m_eState == ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const SGRect worldRect = getWorldBoundingBox();
	const bool bContainedMouse = worldRect.containsPoint(mousePos);

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

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	if (!bContainedMouse) {
		return true;
	}

	m_eState = ePressed;
	return true;
}

bool SGUIElement::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_eState != ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

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

void SGUIElement::updateState() {
}

void SGUIElement::setCallbackClick(const SGActionFn<SGEventMouse*>& callback) {
	m_fnMouseClickCallback = callback;
}

SGRect SGUIElement::getWorldBoundingBox() const {
	SGVec2 origin;

	/*
	 * 그룹마스터와 마스터 그룹들은 비쥬얼 부모(_parent)가 존재하지 않는다.
	 * 그룹마스터는 비쥬얼 부모, 논리 부모 모두 존재하지 않는다. (계층 최상위 노드이기 때문)
	 * 마스터 그룹들은 논리 부모는 존재하지만 비쥬얼 부모는 추후 UILayer에 붙이기 위해 존재하지 않도록 한다.
	 */

	if (_parent == nullptr)	
		origin = getPosition();
	else
		origin = _parent->convertToWorldSpace(getPosition());

	return { origin, getContentSize() };
}

void SGUIElement::setEnabled(bool enabled) {
	m_eState = enabled ? eNormal : eDisabled;
}


SGVec2 SGUIElement::relativePositionInRect(
	const SGRect& rc,
	float origin_x,
	float origin_y) const
{
	float xPos = 0;
	float yPos = 0;

	switch (m_pBaseInfo->HAlignment) {
	case HAlignment::Left:		xPos = 0;											break;
	case HAlignment::Center:	xPos = rc.getWidth() / 2 - getContentWidth() / 2;	break;
	case HAlignment::Right:		xPos = rc.getWidth() - getContentWidth();			break;
	}

	switch (m_pBaseInfo->VAlignment) {
	case VAlignment::Bottom:	yPos = 0;											break;
	case VAlignment::Center:	yPos = rc.getHeight() / 2 - getContentHeight() / 2;	break;
	case VAlignment::Top:		yPos = rc.getHeight() - getContentHeight();			break;
	}

	return { xPos + origin_x, yPos + origin_y };
}


void SGUIElement::setPositionRelative(float x, float y) {
	// 마스터 그룹들은 실제 부모가 없음.
	const SGRect rect = isMasterGroup() ? SGRect{0, 0, CoreInfo_v->ResolutionWidth, CoreInfo_v->ResolutionHeight} : m_pParent->getWorldBoundingBox();
	const SGVec2 realPos = relativePositionInRect(rect, x, y);
	setPosition(realPos);
}

void SGUIElement::setPositionRelative(const SGVec2& pos) {
	setPositionRelative(pos.x, pos.y);
}