/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:56:57 AM
 * =====================
 * 
 */


#pragma once


#include "sg/Struct/SteinsGate_UI.h"
#include "sgcl/Game/Texture/FrameTexture.h"
#include "sg/Sga/SgaSpriteHelper.h"

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

class UIElement : public cc::Node
{
public:
	inline static const cc::size DEFAULT_SIZE45 = { 45, 45 };
	inline static const cc::size DEFAULT_SIZE30 = { 30, 30 };
	inline static const cc::size DEFAULT_SIZE15 = { 15, 15 };
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
	virtual void RefreshThemeVisuals() {}
	virtual bool OnMouseMoveInternal(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseDownInternal(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseUpInternal(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseScrollInternal(cc::EventMouse* _mouseEvent);

	virtual void OnMouseEnterInternalDetail(cc::EventMouse* _mouseEvent);
	virtual void OnMouseLeaveInternalDetail(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseMoveInternalDetail(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseDownInternalDetail(cc::EventMouse* _mouseEvent);
	virtual void OnMouseUpInternalDetail(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseUpContainedInternalDetail(cc::EventMouse* _mouseEvent);
	virtual bool OnMouseScrollInternalDetail(cc::EventMouse* _mouseEvent);

	virtual void SetInfo(UIElementInfo* _info, bool _infoOwner) = 0;
	virtual void SetEnabled(bool _enabled);
	virtual UIElementType_t GetElementType() = 0;

	virtual bool IsGroup() const { return false; }
	virtual bool IsRootGroup() { return false; }

	cc::rect GetWorldBoundingBox() const;
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

	const char* GetName() const { return pBaseInfo_->name_; }
	UIRootGroup* GetRootGroup() const { return pRootGroup_; }

	cc::vec2 CalculateZeroPosition(const cc::rect& _rc) const;
	cc::vec2 CalculateZeroPosition(const cc::rect& _rc, HAlignment_t _halign, VAlignment_t _valign) const;
	cc::vec2 CalculateRelativePosition(const cc::size& _parentSize) const;

	cc::rect GetParentAbsoluteRect();
	cc::rect GetParentRect();
	cc::size GetParentSize();

	float GetAbsoluteScaleX();
	float GetAbsoluteScaleY();
	cc::vec2 GetAbsoluteScale();
	cc::vec2 GetAbsolutePosition() const;


	// getPosition : 그룹내에서 엘리먼트의 좌하단 위치를 반환
	cc::vec2 GetPositionCenter() const; // 그룹내에서 엘리먼트의 중앙 위치를 반환
	cc::vec2 GetPositionRightTop() const; // 그룹내에서 엘리먼트의 우상단 위치를 반환
	cc::vec2 GetRelativePosition();
	cc::vec2 GetRelativePositionOnElement(const cc::vec2& _absolutePos) const;

	void SetRelativePosition(float _x, float _y); // 부모기준 상대적 위치 반영
	void SetRelativePosition(const cc::vec2& _pos); // 부모기준 상대적 위치 반영
	void SetRelativePosition(float _x, float _y, HAlignment_t _halign, VAlignment_t _valign); // 부모기준 상대적 위치 반영
	void SetRelativePosition(UIElement* _pTarget, float _x, float _y, HAlignment_t _halign, VAlignment_t _valign);
	// 타겟기준 상대적 위치 반영


	void InvokeMouseEvent(MouseEventType _mouseEventType, cc::EventMouse* _mouseEvent);
	void AddMouseEvent(MouseEventType _mouseEventType, int _id, const jc::Action<cc::EventMouse*>& _fn);
	void AddMouseEvent(MouseEventType _mouseEventType, int _id, jc::Action<cc::EventMouse*>&& _fn);
	void RemoveMouseEvent(MouseEventType _mouseEventType, int _id);

	void SetUISize(const float _width, const float _height) { SetUISize({ _width, _height }); }
	virtual void SetUISize(const cc::size& _size) { uiSize_ = _size; }
	cc::rect GetUIRect() const { return { cc::vec2{}, uiSize_ }; }
	const cc::size& GetUISize() const { return uiSize_; }

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
	void ApplyUIScaleFactor(OUT cc::size& _size);
	void ApplyUIScaleFactor(OUT cc::vec2& _vec2);
	void ApplyUIScaleFactorX(OUT float& _x);
	void ApplyUIScaleFactorY(OUT float& _y);

	bool IsContainPoint(cc::EventMouse* _mouseEvent);
	virtual void SetInitialUISize(cc::size _size);

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
	cc::size uiSize_; // 화면상 보이는 UI 크기 
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
