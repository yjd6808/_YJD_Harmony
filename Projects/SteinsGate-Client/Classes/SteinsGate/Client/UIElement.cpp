/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 6:35:08 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIElement.h"

#include <SteinsGate/Client/UIRootGroup.h>
#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIElement::UIElement(UIRootGroup* _masterGroup, UIGroup* _parent)
: pMouseEventMap_{}
, pBaseInfo_(nullptr)
, pDragLinkElement_(nullptr)
, pRootGroup_(_masterGroup)
, pParent_(_parent)
, state_(eNormal)
, isDeveloperCreated_(false)
, isInfoOwner_(false)
, isInitialized_(false)
, isDraggable_(false)
, isLoaded_(false)
, isFocused_(false)
, isResizable_(true)
, isInternalDetailEventEnabled_(true)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElement::UIElement(UIRootGroup* _masterGroup, UIGroup* _parent, UIElementInfo* _info, bool _infoOwner)
: UIElement(_masterGroup, _parent)
{
	pBaseInfo_ = _info;
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElement::~UIElement()
{
	for (int i = 0; i < eMouseEventMax; ++i)
	{
		JCORE_DELETE_SAFE(pMouseEventMap_[i]);
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pBaseInfo_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::init()
{
	_cascadeColorEnabled = true;
	_cascadeOpacityEnabled = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::Load()
{
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::Unload()
{
	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::Reload()
{
	Unload();
	Load();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::Loaded() const
{
	return isLoaded_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::Focus()
{
	isFocused_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::Unfocus()
{
	isFocused_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::RestoreState(State _state)
{
	if (state_ == eDisabled)
		return;

	state_ = eNormal;
}


//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::IsContainPoint(SGEventMouse* _mouseEvent)
{
	Vec2 mousePos = _mouseEvent->getCursorPos();

	const Rect boundingBox = {
		_position.x,
		_position.y,
		uiSize_.width,
		uiSize_.height
	};

	return boundingBox.containsPoint(mousePos);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::SetInitialUISize(SGSize _size)
{
	if (isInitialized_)
	{
		_LogWarn_("초기화 수행 후 setInitialUISize 함수 호출 금지");
		return;
	}

	_size.width *= Core::ClientInfo->uiScaleXFactor_;
	_size.height *= Core::ClientInfo->uiScaleYFactor_;

	_contentSize = _size;
	uiSize_ = _size;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::LogWarnMissingInfo()
{
	_LogWarn_("%s::m_pInfo가 초기화안됨.", UIElementType::Name[GetElementType()]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::LogWarnInvalidInfo(UIElementType_t _targetType)
{
	_LogWarn_("%s::setInfo() 실패, 잘못된 타입의 UI 엘리먼트 정보(%s) 주입"
	          , UIElementType::Name[GetElementType()]
	          , UIElementType::Name[_targetType]);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::OnMouseDownInternal(SGEventMouse* _mouseEvent)
{
	if (state_ == eDisabled)
		return true;

	if (!IsContainPoint(_mouseEvent))
	{
		return true;
	}

	pRootGroup_->OnMouseDownTarget(this, _mouseEvent);
	InvokeMouseEvent(eMouseEventDown, _mouseEvent);
	state_ = ePressed;
	bool bPropagate = true;

	if (isInternalDetailEventEnabled_)
		bPropagate = OnMouseDownInternalDetail(_mouseEvent);

	// 마우스를 눌렀을 때는 실질적인 드래그를 수행하지는 않기 때문에
	// 드래그 시작 위치만 계속 업데이트시키도록 한다.
	if (isDraggable_ && !Core::Contents.UIManager->IsDragging())
	{
		DragState dragState;
		dragState.isDragging_ = false; // 아직 실제 드래그가 시작된 상태가 아니므로
		dragState.startCursorPosition_ = _mouseEvent->getStartCursorPos();
		dragState.pHostElement_ = this;
		bPropagate = false;

		// 링크 엘리먼트가 있을 경우 링크 엘리먼트의 위치를 넣는다.
		if (pDragLinkElement_)
		{
			dragState.startElementPosition_ = pDragLinkElement_->_position;
			dragState.pTargetElement_ = pDragLinkElement_;
		}
		else
		{
			dragState.startElementPosition_ = _position;
			dragState.pTargetElement_ = this;
		}

		Core::Contents.UIManager->Draginit(dragState);
	}
	return bPropagate;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::OnMouseMoveInternal(SGEventMouse* _mouseEvent)
{
	const DragState& dragState = Core::Contents.UIManager->GetDragState();

	// 주의사항: 스크롤바에 드래깅 활성화시 손잡이 드래그보다 먼저 Element에서 드래그 체크를 수행하기 때문에 손잡이 드래그가 안먹힌다.
	// 만약 onMouseDown에서 드래그 초기화된 엘리먼트가 있는 경우
	// 엘리먼트에 포함되어있지 않더라도 드래그 중일 경우 따라서 움직여야하므로 제일 먼저 처리하도록 하자.
	if (dragState.isDragging_)
	{
		// 자기자신이 아닌 경우 대상을 찾아야하므로 이벤트는 상위 엘리먼트로 전파되도록 해줘야한다.
		if (dragState.pTargetElement_ != this)
		{
			return true;
		}

		Core::Contents.UIManager->DragMove(_mouseEvent);
		return false;
	}

	// 드래그 중이 아닌데 드래그 타겟인 경우
	if (dragState.pTargetElement_ == this)
	{
		Core::Contents.UIManager->DragEnter(_mouseEvent);
		return false;
	}

	if (state_ == eDisabled)
		return true;

	if (!IsContainPoint(_mouseEvent))
	{
		if (state_ == eOver)
		{
			pRootGroup_->OnMouseLeaveTarget(this, _mouseEvent);

			if (isInternalDetailEventEnabled_)
				OnMouseLeaveInternalDetail(_mouseEvent);

			InvokeMouseEvent(eMouseEventLeave, _mouseEvent);

			// 눌리고있는 상태인 경우, 눌린걸 해제했을때 Normal로 바꿔줘야한다.
			if (state_ != ePressed)
				state_ = eNormal;
		}

		return true;
	}

	if (state_ == eNormal)
	{
		pRootGroup_->OnMouseEnterTarget(this, _mouseEvent);
		InvokeMouseEvent(eMouseEventEnter, _mouseEvent);

		if (isInternalDetailEventEnabled_)
			OnMouseEnterInternalDetail(_mouseEvent);
		state_ = eOver;
	}

	pRootGroup_->OnMouseMoveTarget(this, _mouseEvent);
	InvokeMouseEvent(eMouseEventMove, _mouseEvent);
	const bool bPropagate = OnMouseMoveInternalDetail(_mouseEvent);
	return bPropagate;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::OnMouseUpInternal(SGEventMouse* _mouseEvent)
{
	// 기본적으로 싱글 엘리먼트 드래그만 지원하기 떄문에 마우스를 땠을때 드래그 해제 처리는 WorldScene::onMouseUp에서 처리함

	if (state_ == eDisabled)
		return true;

	if (isInternalDetailEventEnabled_)
		OnMouseUpInternalDetail(_mouseEvent);

	InvokeMouseEvent(eMouseEventUp, _mouseEvent);

	if (!IsContainPoint(_mouseEvent))
	{
		state_ = eNormal;
		return true;
	}

	bool bPropagate = true;

	pRootGroup_->OnMouseUpTarget(this, _mouseEvent);
	InvokeMouseEvent(eMouseEventUpContained, _mouseEvent);

	// 마우스를 땠을때 드래그 중인 상태인 경우 자식 엘리먼트 구현체들(버튼, 스크롤바, 에딧박스..등등)에게 이벤트가 전달되지 않도록 한다.
	if (!Core::Contents.UIManager->IsDragging())
	{
		if (isInternalDetailEventEnabled_)
		{
			bPropagate = OnMouseUpContainedInternalDetail(_mouseEvent);
		}
	}

	state_ = eNormal;
	return bPropagate;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::OnMouseScrollInternal(SGEventMouse* _mouseEvent)
{
	if (state_ == eDisabled)
		return true;

	if (!IsContainPoint(_mouseEvent))
	{
		return true;
	}

	pRootGroup_->OnMouseScrollTarget(this, _mouseEvent);
	InvokeMouseEvent(eMouseEventScroll, _mouseEvent);

	bool bPropagate = true;

	if (isInternalDetailEventEnabled_)
	{
		bPropagate = OnMouseScrollInternalDetail(_mouseEvent);
	}
	return bPropagate;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::OnMouseEnterInternalDetail(SGEventMouse* _mouseEvent)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::OnMouseLeaveInternalDetail(SGEventMouse* _mouseEvent)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::OnMouseMoveInternalDetail(SGEventMouse* _mouseEvent) { return true; }
bool UIElement::OnMouseDownInternalDetail(SGEventMouse* _mouseEvent) { return true; }

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::OnMouseUpInternalDetail(SGEventMouse* _mouseEvent)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIElement::OnMouseUpContainedInternalDetail(SGEventMouse* _mouseEvent) { return true; }
bool UIElement::OnMouseScrollInternalDetail(SGEventMouse* _mouseEvent) { return true; }

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::UpdateState()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 UIElement::GetAbsolutePosition() const
{
	Vec2 thisAbsolutePos = _position;
	UIElement* pCur = pParent_;
	while (pCur != nullptr)
	{
		thisAbsolutePos += pCur->_position;
		pCur = pCur->pParent_;
	}
	return thisAbsolutePos;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 UIElement::GetRelativePositionOnElement(const SGVec2& _absolutePos) const
{
	return _absolutePos - GetAbsolutePosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 UIElement::GetPositionCenter() const
{
	return {
		_position.x + uiSize_.width / 2.0f,
		_position.y + uiSize_.height / 2.0f};
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 UIElement::GetPositionRightTop() const
{
	return {
		_position.x + uiSize_.width,
		_position.y + uiSize_.height,};
}


//////////////////////////////////////////////////////////////////////////////////////////
// getRelativePosition() : 정렬 정보에 따른 상대적 위치 정보를 반환
// 예를들어서 valign: center, halign: center인 경우
//  => 그룹(사각형) 중앙점 기준으로 엘리먼트 중점의 상대적 위치를 반환한다.
// valign: top, halign: left인 경우
//  => 그룹(사각형) 좌상단점 기준으로 엘리먼트 좌상단점의 상대적 위치를 반환한다.
SGVec2 UIElement::GetRelativePosition()
{
	Size parentSize = GetParentSize();
	return CalculateRelativePosition(parentSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGRect UIElement::GetParentAbsoluteRect()
{
	return IsRootGroup() ? Core::ClientInfo->GetGameResolutionRect() : pParent_->GetWorldBoundingBox();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGRect UIElement::GetParentRect()
{
	return IsRootGroup()
		       ? Core::ClientInfo->GetGameResolutionRect()
		       : SGRect{ 0, 0, pParent_->uiSize_.width, pParent_->uiSize_.height };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGSize UIElement::GetParentSize()
{
	return IsRootGroup() ? Core::ClientInfo->gameResolutionSize_ : pParent_->uiSize_;
}

//////////////////////////////////////////////////////////////////////////////////////////
float UIElement::GetAbsoluteScaleX()
{
	float thisScaleX = _scaleX;
	UIElement* pCur = pParent_;
	while (pCur != nullptr)
	{
		thisScaleX *= pCur->_scaleX;
		pCur = pCur->pParent_;
	}
	return thisScaleX;
}

//////////////////////////////////////////////////////////////////////////////////////////
float UIElement::GetAbsoluteScaleY()
{
	float thisScaleY = _scaleY;
	UIElement* pCur = pParent_;
	while (pCur != nullptr)
	{
		thisScaleY *= pCur->_scaleY;
		pCur = pCur->pParent_;
	}
	return thisScaleY;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 UIElement::GetAbsoluteScale()
{
	float thisScaleX = _scaleX;
	float thisScaleY = _scaleY;
	UIElement* pCur = pParent_;
	while (pCur != nullptr)
	{
		thisScaleX *= pParent_->_scaleX;
		thisScaleY *= pParent_->_scaleY;
		pCur = pCur->pParent_;
	}
	return { thisScaleX, thisScaleY };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGRect UIElement::GetWorldBoundingBox() const
{
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

	return { origin, uiSize_ };
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::SetEnabled(bool _enabled)
{
	state_ = _enabled ? eNormal : eDisabled;
}


//////////////////////////////////////////////////////////////////////////////////////////
// 사각형 내부에서 현재 엘리먼트 위치가 0, 0이라는 가정하에 정렬기준에 따른 좌하단 좌표를 얻는다.
// 사각형(보통 부모)내부에서 정렬 정보에 따른 엘리먼트의 좌하단 위치를 반환 (origin은 상대 위치를 의미한다.)
// 예를들어 모두 중앙 정렬된 엘리먼트인 경우의 사각형의 크기가 200, 200이라고 가정하고 origin은 0, 0이라고하자.
// 이때 엘리먼트의 크기가 50 x 40이면
// (100 - 25, 100 - 20)이 엘리먼트의 좌하단 좌표이다.
SGVec2 UIElement::CalculateZeroPosition(const SGRect& _rc) const
{
	return CalculateZeroPosition(_rc, pBaseInfo_->hAlignment_, pBaseInfo_->vAlignment_);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 UIElement::CalculateZeroPosition(const SGRect& _rc, HAlignment_t _halign, VAlignment_t _valign) const
{
	float xPos = 0;
	float yPos = 0;

	switch (_halign)
	{
	case HAlignment::Left: xPos = 0;
		break;
	case HAlignment::Center: xPos = _rc.size.width / 2 - uiSize_.width / 2;
		break;
	case HAlignment::Right: xPos = _rc.size.width - uiSize_.width;
		break;
	}

	switch (_valign)
	{
	case VAlignment::Bottom: yPos = 0;
		break;
	case VAlignment::Center: yPos = _rc.size.height / 2 - uiSize_.height / 2;
		break;
	case VAlignment::Top: yPos = _rc.size.height - uiSize_.height;
		break;
	}

	return { xPos, yPos };
}


//////////////////////////////////////////////////////////////////////////////////////////
// 사이즈 내부에서 현재 엘리먼트의 정렬기준에 따른 위치를 반환한다.
SGVec2 UIElement::CalculateRelativePosition(const SGSize& _parentSize) const
{
	Vec2 pos;
	switch (pBaseInfo_->hAlignment_)
	{
	case HAlignment::Left:
		pos.x = _position.x;
		break;
	case HAlignment::Center:
		pos.x = _position.x + uiSize_.width / 2.0f - _parentSize.width / 2.0f;
		break;
	case HAlignment::Right:
		pos.x = _position.x + uiSize_.width - _parentSize.width;
		break;
	}

	switch (pBaseInfo_->vAlignment_)
	{
	case VAlignment::Bottom:
		pos.y = _position.y;
		break;
	case VAlignment::Center:
		pos.y = _position.y + uiSize_.height / 2.0f - _parentSize.height / 2.0f;
		break;
	case VAlignment::Top:
		pos.y = _position.y + uiSize_.height - _parentSize.height;
		break;
	}
	return pos;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::SetRelativePosition(float _x, float _y)
{
	const Rect rect = { Vec2{}, GetParentSize() };
	const Vec2 realPos = CalculateZeroPosition(rect);
	setPosition(realPos + Vec2{ _x, _y });
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::SetRelativePosition(float _x, float _y, HAlignment_t _halign, VAlignment_t _valign)
{
	const Rect rect = { Vec2{}, GetParentSize() };
	const Vec2 realPos = CalculateZeroPosition(rect, _halign, _valign);
	setPosition(realPos + Vec2{ _x, _y });
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::SetRelativePosition(UIElement* _pTarget, float _x, float _y, HAlignment_t _halign, VAlignment_t _valign)
{
	// 1. 먼저 대상의 0,0좌표로 이 대상을 이동시킨다.
	const Vec2 thisAbsPos = GetAbsolutePosition();
	const Vec2 targetAbsPos = _pTarget->GetAbsolutePosition();
	const Vec2 zeroPos = _position + targetAbsPos - thisAbsPos;
	setPosition(zeroPos);

	// 2. 대상 사각형 기준 상대좌표를 구한다.
	const Rect rect = { Vec2{}, _pTarget->GetUISize() };
	const Vec2 realPos = CalculateZeroPosition(rect, _halign, _valign);
	setPosition(_position + realPos + Vec2{ _x, _y });
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::SetRelativePosition(const SGVec2& _pos)
{
	SetRelativePosition(_pos.x, _pos.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::InvokeMouseEvent(MouseEventType _mouseEventType, cocos2d::EventMouse* _mouseEvent)
{
	if (pMouseEventMap_[_mouseEventType] == nullptr)
	{
		return;
	}

	pMouseEventMap_[_mouseEventType]->Invoke(_mouseEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::AddMouseEvent(MouseEventType _mouseEventType, int _id, const SGActionFn<SGEventMouse*>& _fn)
{
	if (pMouseEventMap_[_mouseEventType] == nullptr)
		pMouseEventMap_[_mouseEventType] = new SGMouseEventList;

	pMouseEventMap_[_mouseEventType]->Register(_id, _fn);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::AddMouseEvent(MouseEventType _mouseEventType, int _id, SGActionFn<SGEventMouse*>&& _fn)
{
	if (pMouseEventMap_[_mouseEventType] == nullptr)
		pMouseEventMap_[_mouseEventType] = new SGMouseEventList;

	pMouseEventMap_[_mouseEventType]->Register(_id, Move(_fn));
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIElement::RemoveMouseEvent(MouseEventType _mouseEventType, int _id)
{
	if (pMouseEventMap_[_mouseEventType] == nullptr)
	{
		return;
	}

	if (!pMouseEventMap_[_mouseEventType]->Unregister(_id))
	{
		_LogWarn_("%s %d 마우스 이벤트를 제거하는데 실패했습니다..", ToString().Source(), _mouseEventType);
	}
}
