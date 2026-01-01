/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:56:57 AM
 * =====================
 * 
 */


#pragma once

#include <sgcl/Core.h>

#include <sgcl/UIInfo.h>
#include <sgcl/FrameTexture.h>
#include <sg/SgaSpriteHelper.h>

enum MouseEventType
{
	eMouseEventDown,
	eMouseEventMove,
	eMouseEventUp,
	eMouseEventUpContained,
	eMouseEventScroll,
	eMouseEventLeave,
	eMouseEventEnter,
	eMouseEventMax
};

using SGMouseEventList = jc::Event<cocos2d::EventMouse*>;

class UIGroup;
class UIRootGroup;

class UIElement : public c2d::Node
{
public:
	inline static const c2d::size DEFAULT_SIZE45 = { 45, 45 };
	inline static const c2d::size DEFAULT_SIZE30 = { 30, 30 };
	inline static const c2d::size DEFAULT_SIZE15 = { 15, 15 };
	static constexpr int MIN_DRAG_START_DISTANCE = 5.0f; // 드래그 시작 최단 거리

	enum State
	{
		eNormal,
		eOver,
		ePressed,
		eDisabled,
		eMax
	};

	UIElement(UIRootGroup* _masterGroup, UIGroup* _parent);
	UIElement(UIRootGroup* _masterGroup, UIGroup* _parent, UIElementInfo* _info, bool _infoOwner);
	~UIElement() override;

	bool init() override;
	bool Loaded() const;

	virtual void Focus();
	virtual void Unfocus();

	virtual void RestoreState(State _state);
	virtual void Load();
	virtual void Unload();
	virtual void Reload();
	virtual bool OnMouseMoveInternal(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseDownInternal(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseUpInternal(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseScrollInternal(c2d::EventMouse* _mouseEvent);

	virtual void OnMouseEnterInternalDetail(c2d::EventMouse* _mouseEvent);
	virtual void OnMouseLeaveInternalDetail(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseMoveInternalDetail(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseDownInternalDetail(c2d::EventMouse* _mouseEvent);
	virtual void OnMouseUpInternalDetail(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseUpContainedInternalDetail(c2d::EventMouse* _mouseEvent);
	virtual bool OnMouseScrollInternalDetail(c2d::EventMouse* _mouseEvent);

	virtual void SetInfo(UIElementInfo* _info, bool _infoOwner) = 0;
	virtual void SetEnabled(bool _enabled);
	virtual UIElementType_t GetElementType() = 0;

	virtual bool IsGroup() const { return false; }
	virtual bool IsRootGroup() { return false; }

	c2d::rect GetWorldBoundingBox() const;
	virtual void UpdateState();

	template <typename TElement>
	TElement Cast()
	{
		static_assert(jc::IsPointerType_v<TElement>, "... TElement must be pointer type");
		static_assert(jc::IsBaseOf_v<UIElement, jc::RemovePointer_t<TElement>>,
		              "... TElement must be UIElement type");
		return static_cast<TElement>(this);
	}

	virtual jc::String ToString() = 0;

	int GetCode() const { return pBaseInfo_->code_; }
	UIRootGroup* GetRootGroup() const { return pRootGroup_; }

	c2d::vec2 CalculateZeroPosition(const c2d::rect& _rc) const;
	c2d::vec2 CalculateZeroPosition(const c2d::rect& _rc, HAlignment_t _halign, VAlignment_t _valign) const;
	c2d::vec2 CalculateRelativePosition(const c2d::size& _parentSize) const;

	c2d::rect GetParentAbsoluteRect();
	c2d::rect GetParentRect();
	c2d::size GetParentSize();

	float GetAbsoluteScaleX();
	float GetAbsoluteScaleY();
	c2d::vec2 GetAbsoluteScale();
	c2d::vec2 GetAbsolutePosition() const;


	// getPosition : 그룹내에서 엘리먼트의 좌하단 위치를 반환
	c2d::vec2 GetPositionCenter() const; // 그룹내에서 엘리먼트의 중앙 위치를 반환
	c2d::vec2 GetPositionRightTop() const; // 그룹내에서 엘리먼트의 우상단 위치를 반환
	c2d::vec2 GetRelativePosition();
	c2d::vec2 GetRelativePositionOnElement(const c2d::vec2& _absolutePos) const;

	void SetRelativePosition(float _x, float _y); // 부모기준 상대적 위치 반영
	void SetRelativePosition(const c2d::vec2& _pos); // 부모기준 상대적 위치 반영
	void SetRelativePosition(float _x, float _y, HAlignment_t _halign, VAlignment_t _valign); // 부모기준 상대적 위치 반영
	void SetRelativePosition(UIElement* _pTarget, float _x, float _y, HAlignment_t _halign, VAlignment_t _valign);
	// 타겟기준 상대적 위치 반영


	void InvokeMouseEvent(MouseEventType _mouseEventType, c2d::EventMouse* _mouseEvent);
	void AddMouseEvent(MouseEventType _mouseEventType, int _id, const jc::Action<c2d::EventMouse*>& _fn);
	void AddMouseEvent(MouseEventType _mouseEventType, int _id, jc::Action<c2d::EventMouse*>&& _fn);
	void RemoveMouseEvent(MouseEventType _mouseEventType, int _id);

	void SetUISize(const float _width, const float _height) { SetUISize({ _width, _height }); }
	virtual void SetUISize(const c2d::size& _size) { uiSize_ = _size; }
	c2d::rect GetUIRect() const { return { c2d::vec2{}, uiSize_ }; }
	const c2d::size& GetUISize() const { return uiSize_; }

	void SetUIScale(float _scale) { SetUISize({ _contentSize.width * _scale, _contentSize.height * _scale }); }
	void SetUIScale(float _scaleX, float _scaleY) { SetUISize({ _contentSize.width * _scaleX, _contentSize.height * _scaleY }); }
	void SetUIScaleX(float _scaleX) { SetUISize({ _contentSize.width * _scaleX, uiSize_.height }); }
	void SetUIScaleY(float _scaleY) { SetUISize({ uiSize_.width, _contentSize.height * _scaleY }); }
	float GetUIScaleX() { return uiSize_.width / _contentSize.width; }
	float GetUIScaleY() { return uiSize_.height / _contentSize.height; }

	void SetResizable(bool _resizable) { isResizable_ = _resizable; }
	bool IsResizable() const { return isResizable_; }

	void SetDraggable(bool _draggable) { isDraggable_ = _draggable; }
	bool IsDraggable() const { return isDraggable_; }

	void SetDeveloperCreated(bool _developerCreated) { isDeveloperCreated_ = _developerCreated; }
	bool IsDeveloperCreated() const { return isDeveloperCreated_; }

	void SetDragLinkElement(UIElement* _dragLinkElement) { pDragLinkElement_ = _dragLinkElement; }
	void SetInternalDetailEventEnabled(bool _enabledInternalDetailEnabled) { isInternalDetailEventEnabled_ = _enabledInternalDetailEnabled; }

protected:
	void ApplyUIScaleFactor(OUT c2d::size& _size);
	void ApplyUIScaleFactor(OUT c2d::vec2& _vec2);
	void ApplyUIScaleFactorX(OUT float& _x);
	void ApplyUIScaleFactorY(OUT float& _y);

	bool IsContainPoint(c2d::EventMouse* _mouseEvent);
	virtual void SetInitialUISize(c2d::size _size);

	void LogWarnMissingInfo();
	void LogWarnInvalidInfo(UIElementType_t _targetType);

	// 현재 내가 정립한 UI 규격
	// m_pInfo->Size = 실제 UI 크기(설정파일)
	// _contentSize = 게임 해상도가 반영된 UI 크기
	// m_UISize = 사용자 지정 변경된 UI 크기
	// _position = 부모 노드 기준 좌하단 좌표

	SGMouseEventList* pMouseEventMap_[eMouseEventMax];
	UIElementInfo* pBaseInfo_;
	UIElement* pDragLinkElement_;
	// 이 엘리먼트를 드래그할 때 실제로 움직일 엘리먼트 (이 엘리먼트와, 링크 엘리먼트 모두 Draggable 옵션이 활성화되어 있어야함.) 추가한 이유: 타이틀바를 잡고 드래그할 때 그룹 전체를 움직이기 위함.
	UIRootGroup* pRootGroup_; // 대통령
	UIGroup* pParent_; // 직속 상관
	c2d::size uiSize_; // 화면상 보이는 UI 크기 
	State state_;

	bool isDeveloperCreated_; // 개발자가 수동으로 생성한 커스텀 객체인 경우 (해당 엘리먼트를 포함하는 그룹이 unload될 때 소멸됨)
	bool isInfoOwner_; // m_pBaseInfo의 주인인지
	bool isInitialized_;
	bool isDraggable_;
	bool isLoaded_;
	bool isFocused_;
	bool isResizable_; // 처음 크기가 결정된 후 UIGroup::setContentSize() 호출시 크기 업데이트가 이뤄질지 여부
	bool isInternalDetailEventEnabled_; // 구현된 엘리먼트(UIButton, UICheckBox 같은)에 디테일 이벤트(~~~InternalDetail 함수) 전달할지
};
