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
	: m_pMouseEventMap{}
	, m_pBaseInfo(info)
	, m_pDragLinkElement(nullptr)
	, m_pMasterGroup(masterGroup)
	, m_pParent(parent)
	, m_eState(eNormal)
	, m_bDevloperCreated(false)
	, m_bInitialized(false)
	, m_bDraggable(false)
	, m_bLoaded(false)
	, m_bFocused(false)
	, m_bResizable(true)
{}

UIElement::~UIElement() {
	for (int i = 0; i < eMouseEventMax; ++i) {
		JCORE_DELETE_SAFE(m_pMouseEventMap[i]);
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
	
	Vec2 mousePos = mouseEvent->getCursorPos();

	const Rect boundingBox = {
		_position.x,
		_position.y,
		m_UISize.width,
		m_UISize.height
	};

	return boundingBox.containsPoint(mousePos);
}

void UIElement::setInitialUISize(SGSize size) {
	if (m_bInitialized) {
		_LogWarn_("초기화 수행 후 setInitialUISize 함수 호출 금지");
		return;
	}

	size.width *= CoreClientInfo_v->UIScaleXFactor;
	size.height *= CoreClientInfo_v->UIScaleYFactor;

	_contentSize = size;
	m_UISize = size;
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
	bool bPropagate = onMouseDownDetail(mouseEvent);

	// 마우스를 눌렀을 때는 실질적인 드래그를 수행하지는 않기 때문에
	// 드래그 시작 위치만 계속 업데이트시키도록 한다.
	if (m_bDraggable && !CoreUIManager_v->isDragging()) {
		DragState dragState;
		dragState.Dragging = false;	// 아직 실제 드래그가 시작된 상태가 아니므로
		dragState.StartCursorPosition = mouseEvent->getStartCursorPos();
		dragState.HostElement = this;
		bPropagate = false;
		
		// 링크 엘리먼트가 있을 경우 링크 엘리먼트의 위치를 넣는다.
		if (m_pDragLinkElement) {
			dragState.StartElementPosition = m_pDragLinkElement->_position;
			dragState.TargetElement = m_pDragLinkElement;
		} else {
			dragState.StartElementPosition = _position;
			dragState.TargetElement = this;
		}

		CoreUIManager_v->draginit(dragState);
	}
	return bPropagate;
}


bool UIElement::onMouseMove(SGEventMouse* mouseEvent) {


	const DragState& dragState = CoreUIManager_v->getDragState();

	// 주의사항: 스크롤바에 드래깅 활성화시 손잡이 드래그보다 먼저 Element에서 드래그 체크를 수행하기 때문에 손잡이 드래그가 안먹힌다.
	// 만약 onMouseDown에서 드래그 초기화된 엘리먼트가 있는 경우
	// 엘리먼트에 포함되어있지 않더라도 드래그 중일 경우 따라서 움직여야하므로 제일 먼저 처리하도록 하자.
	if (dragState.Dragging) {

		// 자기자신이 아닌 경우 대상을 찾아야하므로 이벤트는 상위 엘리먼트로 전파되도록 해줘야한다.
		if (dragState.TargetElement != this) {
			return true;
		}

		CoreUIManager_v->dragMove(mouseEvent);
		return false;
	}

	// 드래그 중이 아닌데 드래그 타겟인 경우
	if (dragState.TargetElement == this) {	
		CoreUIManager_v->dragEnter(mouseEvent);
		return false;
	}

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
	const bool bPropagate = onMouseMoveDetail(mouseEvent);
	return bPropagate;
}


bool UIElement::onMouseUp(SGEventMouse* mouseEvent) {
	// 기본적으로 싱글 엘리먼트 드래그만 지원하기 떄문에 마우스를 땠을때 드래그 해제 처리는 WorldScene::onMouseUp에서 처리함
	
	if (m_eState == eDisabled)
		return true;

	onMouseUpDetail(mouseEvent);
	invokeMouseEvent(eMouseEventUp, mouseEvent);

	if (!isContainPoint(mouseEvent)) {
		m_eState = eNormal;
		return true;
	}

	bool bPropagate = true;
	
	m_pMasterGroup->onMouseUp(this, mouseEvent);
	invokeMouseEvent(eMouseEventUpContained, mouseEvent);

	// 마우스를 땠을때 드래그 중인 상태인 경우 자식 엘리먼트 구현체들(버튼, 스크롤바, 에딧박스..등등)에게 이벤트가 전달되지 않도록 한다.
	if (!CoreUIManager_v->isDragging()) {
		bPropagate = onMouseUpContainedDetail(mouseEvent);
	}

	m_eState = eNormal;
	return bPropagate;
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
	Vec2 thisAbsolutePos = _position;
	UIElement* pCur = m_pParent;
	while (pCur != nullptr) {
		thisAbsolutePos += pCur->_position;
		pCur = pCur->m_pParent;
	}
	return thisAbsolutePos;
}

SGVec2 UIElement::getRelativePositionOnElement(SGVec2 absolutePos) const {
	return absolutePos - getAbsolutePosition();
}

SGVec2 UIElement::getPositionCenter() const {
	return {
		_position.x + m_UISize.width / 2.0f,
		_position.y + m_UISize.height / 2.0f,
	};
}

SGVec2 UIElement::getPositionRightTop() const {
	return {
		_position.x + m_UISize.width,
		_position.y + m_UISize.height,
	};
}


// getRelativePosition() : 정렬 정보에 따른 상대적 위치 정보를 반환
// 예를들어서 valign: center, halign: center인 경우
//  => 그룹(사각형) 중앙점 기준으로 엘리먼트 중점의 상대적 위치를 반환한다.
// valign: top, halign: left인 경우
//  => 그룹(사각형) 좌상단점 기준으로 엘리먼트 좌상단점의 상대적 위치를 반환한다.
SGVec2 UIElement::getRelativePosition() {
	Size parentSize = getParentSize();
	return calculateRelativePosition(parentSize);
}

SGRect UIElement::getParentAbsoluteRect() {
	return isMasterGroup() ? CoreClientInfo_v->getGameResolutionRect() : m_pParent->getWorldBoundingBox();
}

SGRect UIElement::getParentRect() {
	return isMasterGroup() ? 
		CoreClientInfo_v->getGameResolutionRect() :
		SGRect{ 0, 0, m_pParent->m_UISize.width, m_pParent->m_UISize.height };
}

SGSize UIElement::getParentSize() {
	return isMasterGroup() ?
		CoreClientInfo_v->GameResolutionSize :
		m_pParent->m_UISize;
}

float UIElement::getAbsoluteScaleX() {
	float thisScaleX = _scaleX;
	UIElement* pCur = m_pParent;
	while (pCur != nullptr) {
		thisScaleX *= pCur->_scaleX;
		pCur = pCur->m_pParent;
	}
	return thisScaleX;
}

float UIElement::getAbsoluteScaleY() {
	float thisScaleY = _scaleY;
	UIElement* pCur = m_pParent;
	while (pCur != nullptr) {
		thisScaleY *= pCur->_scaleY;
		pCur = pCur->m_pParent;
	}
	return thisScaleY;
}

SGVec2 UIElement::getAbsoluteScale() {
	float thisScaleX = _scaleX;
	float thisScaleY = _scaleY;
	UIElement* pCur = m_pParent;
	while (pCur != nullptr) {
		thisScaleX *= m_pParent->_scaleX;
		thisScaleY *= m_pParent->_scaleY;
		pCur = pCur->m_pParent;
	}
	return { thisScaleX, thisScaleY };
}

SGRect UIElement::getWorldBoundingBox() const {
	SGVec2 origin;

	/*
	 * 그룹마스터와 마스터 그룹들은 비쥬얼 부모(_parent)가 존재하지 않는다.
	 * 그룹마스터는 비쥬얼 부모, 논리 부모 모두 존재하지 않는다. (계층 최상위 노드이기 때문)
	 * 마스터 그룹들은 논리 부모는 존재하지만 비쥬얼 부모는 추후 UILayer에 붙이기 위해 존재하지 않도록 한다.
	 */

	if (_parent == nullptr)	
		origin = _position;
	else
		origin = _parent->convertToWorldSpace(_position);

	return { origin, m_UISize };
}

void UIElement::setEnabled(bool enabled) {
	m_eState = enabled ? eNormal : eDisabled;
}


// 사각형 내부에서 현재 엘리먼트 위치가 0, 0이라는 가정하에 정렬기준에 따른 좌하단 좌표를 얻는다.
// 사각형(보통 부모)내부에서 정렬 정보에 따른 엘리먼트의 좌하단 위치를 반환 (origin은 상대 위치를 의미한다.)
// 예를들어 모두 중앙 정렬된 엘리먼트인 경우의 사각형의 크기가 200, 200이라고 가정하고 origin은 0, 0이라고하자.
// 이때 엘리먼트의 크기가 50 x 40이면
// (100 - 25, 100 - 20)이 엘리먼트의 좌하단 좌표이다.
SGVec2 UIElement::calculateZeroPosition(const SGRect& rc) const {
	return calculateZeroPosition(rc, m_pBaseInfo->HAlignment, m_pBaseInfo->VAlignment);
}

SGVec2 UIElement::calculateZeroPosition(const SGRect& rc, HAlignment_t halign, VAlignment_t valign) const {
	float xPos = 0;
	float yPos = 0;

	switch (halign) {
	case HAlignment::Left:		xPos = 0;											break;
	case HAlignment::Center:	xPos = rc.size.width / 2 - m_UISize.width / 2;	break;
	case HAlignment::Right:		xPos = rc.size.width - m_UISize.width;			break;
	}

	switch (valign) {
	case VAlignment::Bottom:	yPos = 0;												break;
	case VAlignment::Center:	yPos = rc.size.height / 2 - m_UISize.height / 2;	break;
	case VAlignment::Top:		yPos = rc.size.height - m_UISize.height;			break;
	}

	return { xPos, yPos };
}


// 사이즈 내부에서 현재 엘리먼트의 정렬기준에 따른 위치를 반환한다.
SGVec2 UIElement::calculateRelativePosition(const SGSize& parentSize) const {
	Vec2 pos;
	switch (m_pBaseInfo->HAlignment) {
	case HAlignment::Left:
		pos.x = _position.x;
		break;
	case HAlignment::Center:
		pos.x = _position.x + m_UISize.width / 2.0f - parentSize.width / 2.0f;
		break;
	case HAlignment::Right:
		pos.x = _position.x + m_UISize.width - parentSize.width;
		break;
	}

	switch (m_pBaseInfo->VAlignment) {
	case VAlignment::Bottom:
		pos.y = _position.y;
		break;
	case VAlignment::Center:
		pos.y = _position.y + m_UISize.height / 2.0f - parentSize.height / 2.0f;
		break;
	case VAlignment::Top:
		pos.y = _position.y + m_UISize.height - parentSize.height;
		break;
	}
	return pos;
}


void UIElement::setRelativePosition(float x, float y) {
	const Rect rect = { Vec2{}, getParentSize() };
	const Vec2 realPos = calculateZeroPosition(rect);
	setPosition(realPos + Vec2{x, y});
}

void UIElement::setRelativePosition(float x, float y, HAlignment_t halign, VAlignment_t valign) {
	const Rect rect = { Vec2{}, getParentSize() };
	const Vec2 realPos = calculateZeroPosition(rect, halign, valign);
	setPosition(realPos + Vec2{ x, y });
}

void UIElement::setRelativePosition(const SGVec2& pos) {
	setRelativePosition(pos.x, pos.y);
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