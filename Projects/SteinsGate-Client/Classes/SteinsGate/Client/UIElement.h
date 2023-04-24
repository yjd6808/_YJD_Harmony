/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:56:57 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

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



class UIGroup;
class UIMasterGroup;
class UIElement : public SGNode
{
public:
	inline static const SGSize DefaultSize45 = { 45, 45 };
	inline static const SGSize DefaultSize30 = { 30, 30 };
	inline static const SGSize DefaultSize15 = { 15, 15 };

	enum State
	{
		eNormal,
		eOver,
		ePressed,
		eDisabled,
		eMax
	};

	UIElement(UIMasterGroup* masterGroup, UIGroup* parent, UIElementInfo* info);
	~UIElement() override;

	bool loaded() const;

	virtual void focus();
	virtual void unfocus();

	virtual void restoreState(State state);
	virtual void load();
	virtual void unload();
	virtual bool onMouseMove(SGEventMouse* mouseEvent);
	virtual bool onMouseDown(SGEventMouse* mouseEvent);
	virtual bool onMouseUp(SGEventMouse* mouseEvent);
	virtual bool onMouseScroll(SGEventMouse* mouseEvent);

	virtual void onMouseEnterDetail(SGEventMouse* mouseEvent);
	virtual void onMouseLeaveDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseMoveDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseDownDetail(SGEventMouse* mouseEvent);
	virtual void onMouseUpDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseUpContainedDetail(SGEventMouse* mouseEvent);
	virtual bool onMouseScrollDetail(SGEventMouse* mouseEvent);

	virtual void setEnabled(bool enabled);
	virtual UIElementType_t getElementType() = 0;

	virtual bool isGroup() const { return false; }
	virtual bool isMasterGroup() { return false; }

	SGRect getWorldBoundingBox() const;
	virtual void updateState();

	template <typename TElem>
	TElem cast() {
		static_assert(JCore::IsPointerType_v<TElem>, "... TElem must be pointer type");
		static_assert(JCore::IsBaseOf_v<UIElement, JCore::RemovePointer_t<TElem>>, "... TElem must be SGUIElement type");
		return static_cast<TElem>(this);
	}

	virtual SGString toString() = 0;

	int getCode() const { return m_pBaseInfo->Code; }

	SGVec2 getAbsolutePosition() const;
	SGVec2 getRelativePositionOnElement(SGVec2 absolutePos) const;
	SGVec2 getRelativePositionInRect(const SGRect& rc, float origin_x, float origin_y) const;

	void setPositionRelative(float x, float y);		// 부모가 그룹이면 그룹 내에서 상대적 위치 반영
	void setPositionRelative(const SGVec2& pos);

	void invokeMouseEvent(MouseEventType mouseEventType, SGEventMouse* mouseEvent);
	void addMouseEvent(MouseEventType mouseEventType, const SGActionFn<SGEventMouse*>& fn);
	void removeMouseEvent(MouseEventType mouseEventType, const SGActionFn<SGEventMouse*>& fn);
protected:
	bool isContainPoint(SGEventMouse* mouseEvent);

	SGMouseEventList* m_pMouseEventMap[eMouseEventMax];
	UIElementInfo* m_pBaseInfo;
	UIMasterGroup* m_pMasterGroup;	// 대통령
	UIGroup* m_pParent;				// 직속 상관
	SGVec2 m_AbsolutePosition;
	State m_eState;

	bool m_bLoaded;
	bool m_bFocused;
};


