/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 6:35:08 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIElement.h"

#include <SteinsGate/Client/UIMasterGroup.h>
#include <SteinsGate/Client/UIDefine.h>

USING_NS_CC;
USING_NS_JC;

UIElement::UIElement(UIMasterGroup* masterGroup, UIGroup* parent, UIElementInfo* info)
	: m_pBaseInfo(info)
	, m_pMasterGroup(masterGroup)
	, m_pParent(parent)
	, m_eState(eNormal)
	, m_bLoaded(false)
	, m_bFocused(false)
	, m_pMouseEventMap{}
{}

UIElement::~UIElement() {
	for (int i = 0; i < eMouseEventMax; ++i) {
		DeleteSafe(m_pMouseEventMap[i]);
	}
}

void UIElement::load() {
	m_bLoaded = true;
}

void UIElement::unload() {
	m_bLoaded = false;
}

bool UIElement::loaded() const {
	return m_bLoaded;
}

void UIElement::focus() {
	m_bFocused = true;
}

void UIElement::unfocus() {
	m_bFocused = false;
}

void UIElement::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	m_eState = eNormal;
}


bool UIElement::isContainPoint(SGEventMouse* mouseEvent) {
	const Vec2 mousePos = mouseEvent->getCursorPos();
	const Rect boundingBox = { _position.x, _position.y, _contentSize.width, _contentSize.height };
	return boundingBox.containsPoint(mousePos);
}


bool UIElement::onMouseMove(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled)
		return true;

	if (!isContainPoint(mouseEvent)) {

		if (m_eState == eOver) {
			m_pMasterGroup->onMouseLeave(this, mouseEvent);
			onMouseLeaveDetail(mouseEvent);
			invokeMouseEvent(eMouseEventLeave, mouseEvent);

			// 눌리고있는 상태인 경우, 눌린걸 해제했을때 Normal로 바꿔줘야한다.
			if (m_eState != ePressed)
				m_eState = eNormal;
		}

		return true;
	}

	if (m_eState == eNormal) {
		m_pMasterGroup->onMouseEnter(this, mouseEvent);
		invokeMouseEvent(eMouseEventEnter, mouseEvent);
		onMouseEnterDetail(mouseEvent);
		m_eState = eOver;
		
	}

	m_pMasterGroup->onMouseMove(this, mouseEvent);
	invokeMouseEvent(eMouseEventMove, mouseEvent);
	return onMouseMoveDetail(mouseEvent);
}

bool UIElement::onMouseDown(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled)
		return true;

	if (!isContainPoint(mouseEvent)) {
		return true;
	}

	m_pMasterGroup->onMouseDown(this, mouseEvent);
	invokeMouseEvent(eMouseEventDown, mouseEvent);
	m_eState = ePressed;
	return onMouseDownDetail(mouseEvent);
}

bool UIElement::onMouseUp(SGEventMouse* mouseEvent) {
	onMouseUpDetail(mouseEvent);
	invokeMouseEvent(eMouseEventUp, mouseEvent);

	if (!isContainPoint(mouseEvent)) {
		m_eState = eNormal;
		return true;
	}

	if (m_eState == ePressed) {
		m_pMasterGroup->onMouseUp(this, mouseEvent);
		invokeMouseEvent(eMouseEventUpContained, mouseEvent);
		const bool bPropagate = onMouseUpContainedDetail(mouseEvent);

		m_eState = eNormal;
		return bPropagate;
	}

	return true;
}

bool UIElement::onMouseScroll(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled)
		return true;

	if (!isContainPoint(mouseEvent)) {
		return true;
	}

	m_pMasterGroup->onMouseScroll(this, mouseEvent);
	invokeMouseEvent(eMouseEventScroll, mouseEvent);
	return onMouseScrollDetail(mouseEvent);
}

void UIElement::onMouseEnterDetail(SGEventMouse* mouseEvent) {}
void UIElement::onMouseLeaveDetail(SGEventMouse* mouseEvent) {}
bool UIElement::onMouseMoveDetail(SGEventMouse* mouseEvent) { return true; }
bool UIElement::onMouseDownDetail(SGEventMouse* mouseEvent) { return true; }
void UIElement::onMouseUpDetail(SGEventMouse* mouseEvent) {}
bool UIElement::onMouseUpContainedDetail(SGEventMouse* mouseEvent) { return true; }
bool UIElement::onMouseScrollDetail(SGEventMouse* mouseEvent) { return true; }

void UIElement::updateState() {}

SGVec2 UIElement::getAbsolutePosition() const {
	SGVec2 thisAbsolutePos = getPosition();
	while (m_pParent != nullptr) {
		thisAbsolutePos += m_pParent->getPosition();
	}
	return thisAbsolutePos;
}

SGVec2 UIElement::getRelativePositionOnElement(SGVec2 absolutePos) const {
	return absolutePos - getAbsolutePosition();
}

SGRect UIElement::getWorldBoundingBox() const {
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

void UIElement::setEnabled(bool enabled) {
	m_eState = enabled ? eNormal : eDisabled;
}


SGVec2 UIElement::getRelativePositionInRect(
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


void UIElement::setPositionRelative(float x, float y) {
	// 마스터 그룹들은 실제 부모가 없음.
	const SGRect rect = isMasterGroup() ? CoreClientInfo_v->getGameResolutionRect() : m_pParent->getWorldBoundingBox();
	const SGVec2 realPos = getRelativePositionInRect(rect, x, y);
	setPosition(realPos);
}

void UIElement::setPositionRelative(const SGVec2& pos) {
	setPositionRelative(pos.x, pos.y);
}

void UIElement::invokeMouseEvent(MouseEventType mouseEventType, SGEventMouse* mouseEvent) {
	if (m_pMouseEventMap[mouseEventType] == nullptr) {
		return;
	}

	m_pMouseEventMap[mouseEventType]->Invoke(mouseEvent);
}

void UIElement::addMouseEvent(MouseEventType mouseEventType, const SGActionFn<SGEventMouse*>& fn) {
	if (m_pMouseEventMap[mouseEventType] == nullptr)
		m_pMouseEventMap[mouseEventType] = new SGMouseEventList;

	m_pMouseEventMap[mouseEventType]->Register(fn);
}

void UIElement::removeMouseEvent(MouseEventType mouseEventType, const SGActionFn<SGEventMouse*>& fn) {
	if (m_pMouseEventMap[mouseEventType] == nullptr) {
		return;
	}

	if (!m_pMouseEventMap[mouseEventType]->Unregister(fn)) {
		_LogWarn_("%s %d 마우스 이벤트를 제거하는데 실패했습니다..", toString().Source(), mouseEventType);
	}
}
