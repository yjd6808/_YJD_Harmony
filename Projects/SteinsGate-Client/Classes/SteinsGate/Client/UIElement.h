/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:56:57 AM
 * =====================
 * 
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <SteinsGate/Client/UIInfo.h>
#include <SteinsGate/Client/FrameTexture.h>
#include <SteinsGate/Common/SgaSpriteHelper.h>

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


using SGMouseEventList = JCore::Event<cocos2d::EventMouse*>;


class UIGroup;
class UIMasterGroup;
class UIElement : public SGNode
{
public:
	inline static const SGSize DefaultSize45 = { 45, 45 };
	inline static const SGSize DefaultSize30 = { 30, 30 };
	inline static const SGSize DefaultSize15 = { 15, 15 };
	inline static const int MinDragStartDistance = 5.0f;		// 드래그 시작 최단 거리

	enum State
	{
		eNormal,
		eOver,
		ePressed,
		eDisabled,
		eMax
	};

	UIElement(UIMasterGroup* masterGroup, UIGroup* parent);
	UIElement(UIMasterGroup* masterGroup, UIGroup* parent, UIElementInfo* info, bool infoOwner);
	~UIElement() override;

	bool init() override;
	bool loaded() const;

	virtual void focus();
	virtual void unfocus();

	virtual void restoreState(State state);
	virtual void load();
	virtual void unload();
	virtual void reload();
	virtual bool onMouseMoveInternal(SGEventMouse* mouseEvent);
	virtual bool onMouseDownInternal(SGEventMouse* mouseEvent);
	virtual bool onMouseUpInternal(SGEventMouse* mouseEvent);
	virtual bool onMouseScrollInternal(SGEventMouse* mouseEvent);

	virtual void onMouseEnterInternalDetail(SGEventMouse* mouseEvent);
	virtual void onMouseLeaveInternalDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseMoveInternalDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseDownInternalDetail(SGEventMouse* mouseEvent);
	virtual void onMouseUpInternalDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseUpContainedInternalDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseScrollInternalDetail(SGEventMouse* mouseEvent);

	virtual void setInfo(UIElementInfo* info, bool infoOwner) = 0;
	virtual void setEnabled(bool enabled);
	virtual UIElementType_t getElementType() = 0;

	virtual bool isGroup() const { return false; }
	virtual bool isMasterGroup() { return false; }

	SGRect getWorldBoundingBox() const;
	virtual void updateState();

	template <typename TElement>
	TElement cast() {
		static_assert(JCore::IsPointerType_v<TElement>, "... TElement must be pointer type");
		static_assert(JCore::IsBaseOf_v<UIElement, JCore::RemovePointer_t<TElement>>, "... TElement must be UIElement type");
		return static_cast<TElement>(this);
	}

	virtual SGString toString() = 0;

	int getCode() const { return m_pBaseInfo->Code; }
	UIMasterGroup* getMasterGroup() const { return m_pMasterGroup; }
	
	SGVec2 calculateZeroPosition(const SGRect& rc) const;
	SGVec2 calculateZeroPosition(const SGRect& rc, HAlignment_t halign, VAlignment_t valign) const;
	SGVec2 calculateRelativePosition(const SGSize& parentSize) const;

	SGRect getParentAbsoluteRect();
	SGRect getParentRect();
	SGSize getParentSize();

	float getAbsoluteScaleX();
	float getAbsoluteScaleY();
	SGVec2 getAbsoluteScale();
	SGVec2 getAbsolutePosition() const;
	

	// getPosition : 그룹내에서 엘리먼트의 좌하단 위치를 반환
	SGVec2 getPositionCenter() const;	// 그룹내에서 엘리먼트의 중앙 위치를 반환
	SGVec2 getPositionRightTop() const;	// 그룹내에서 엘리먼트의 우상단 위치를 반환
	SGVec2 getRelativePosition();
	SGVec2 getRelativePositionOnElement(SGVec2 absolutePos) const;

	void setRelativePosition(float x, float y);																	// 부모기준 상대적 위치 반영
	void setRelativePosition(const SGVec2& pos);																// 부모기준 상대적 위치 반영
	void setRelativePosition(float x, float y, HAlignment_t halign, VAlignment_t valign);						// 부모기준 상대적 위치 반영
	void setRelativePosition(UIElement* target, float x, float y, HAlignment_t halign, VAlignment_t valign);	// 타겟기준 상대적 위치 반영
	

	void invokeMouseEvent(MouseEventType mouseEventType, SGEventMouse* mouseEvent);
	void addMouseEvent(MouseEventType mouseEventType, int id, const SGActionFn<SGEventMouse*>& fn);
	void addMouseEvent(MouseEventType mouseEventType, int id, SGActionFn<SGEventMouse*>&& fn);
	void removeMouseEvent(MouseEventType mouseEventType, int id);

	void setUISize(const float width, const float height) { setUISize({ width, height }); }
	virtual void setUISize(const SGSize& size) { m_UISize = size; }
	SGRect getUIRect() const { return { SGVec2{}, m_UISize }; }
	const SGSize& getUISize() const { return m_UISize; }

	void setUIScale(float scale)				{ setUISize({ _contentSize.width * scale, _contentSize.height * scale }); }
	void setUIScale(float scaleX, float scaleY)	{ setUISize({ _contentSize.width * scaleX, _contentSize.height * scaleY }); }
	void setUIScaleX(float scaleX)				{ setUISize({ _contentSize.width * scaleX, m_UISize.height }); }
	void setUIScaleY(float scaleY)				{ setUISize({ m_UISize.width, _contentSize.height * scaleY }); }

	float getUIScaleX()							{ return m_UISize.width / _contentSize.width; }
	float getUIScaleY()							{ return m_UISize.height / _contentSize.height; }

	void setResizable(bool resizable) { m_bResizable = resizable; }
	bool isResizable() const { return m_bResizable; }

	void setDraggable(bool draggable) { m_bDraggable = draggable; }
	bool isDraggable() const { return m_bDraggable; }

	void setDeveloperCreated(bool developerCreated) { m_bDevloperCreated = developerCreated; }
	bool isDeveloperCreated() const { return m_bDevloperCreated; }

	void setDragLinkElement(UIElement* dragLinkElement) { m_pDragLinkElement = dragLinkElement; }

	void setInternalDetailEventEnabled(bool enabledInternalDetailEnabled) { m_bInternalDetailEventEnabled = enabledInternalDetailEnabled; }
protected:
	bool isContainPoint(SGEventMouse* mouseEvent);
	virtual void setInitialUISize(SGSize size);

	void logWarnMissingInfo();
	void logWarnInvalidInfo(UIElementType_t targetType);

	// 현재 내가 정립한 UI 규격
	// m_pInfo->Size = 실제 UI 크기(설정파일)
	// _contentSize = 게임 해상도가 반영된 UI 크기
	// m_UISize = 사용자 지정 변경된 UI 크기
	// _position = 부모 노드 기준 좌하단 좌표

	SGMouseEventList* m_pMouseEventMap[eMouseEventMax];
	UIElementInfo* m_pBaseInfo;
	UIElement* m_pDragLinkElement;	// 이 엘리먼트를 드래그할 때 실제로 움직일 엘리먼트 (이 엘리먼트와, 링크 엘리먼트 모두 Draggable 옵션이 활성화되어 있어야함.) 추가한 이유: 타이틀바를 잡고 드래그할 때 그룹 전체를 움직이기 위함.
	UIMasterGroup* m_pMasterGroup;	// 대통령
	UIGroup* m_pParent;				// 직속 상관
	SGSize m_UISize;				// 화면상 보이는 UI 크기 
	State m_eState;

	bool m_bDevloperCreated;		// 개발자가 수동으로 생성한 커스텀 객체인 경우 (해당 엘리먼트를 포함하는 그룹이 unload될 때 소멸됨)
	bool m_bInfoOwner;				// m_pBaseInfo의 주인인지
	bool m_bInitialized;
	bool m_bDraggable;
	bool m_bLoaded;
	bool m_bFocused;
	bool m_bResizable;					// 처음 크기가 결정된 후 UIGroup::setContentSize() 호출시 크기 업데이트가 이뤄질지 여부
	bool m_bInternalDetailEventEnabled; // 구현된 엘리먼트(UIButton, UICheckBox 같은)에 디테일 이벤트(~~~InternalDetail 함수) 전달할지
	
};




