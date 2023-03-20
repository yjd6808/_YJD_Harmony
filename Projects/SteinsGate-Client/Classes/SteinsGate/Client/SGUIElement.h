/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:56:57 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Client/SGUIInfo.h>
#include <SteinsGate/Client/SGFrameTexture.h>
#include <SteinsGate/Common/SgaSpriteHelper.h>

class SGUIGroup;
class SGUIElement : public SGNode
{
public:
	enum State
	{
		eNormal,
		eOver,
		ePressed,
		eDisabled,
		eMax
	};

	SGUIElement(SGUIGroup* parent, SGUIElementInfo* info);
	~SGUIElement() override = default;

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
	
	virtual int getCode() = 0;
	virtual void setEnabled(bool enabled);
	virtual UIElementType_t getElementType() = 0;

	SGRect getWorldBoundingBox() const;
	static void updateState();

	void setCallbackClick(const SGActionFn<SGEventMouse*>& callback);

	virtual bool isGroup() const { return false; }
	virtual bool isGroupMaster() const { return false; }
	virtual bool isMasterGroup() { return false; }

	// 정렬 방식에 따라 앵커포인트 0, 0기준으로해서 박스내의 위치를 구한다.
	SGVec2 getPositionInRect(
		const SGRect& rc,
		float origin_x,
		float origin_y) const;


	void setPositionRelative(float x, float y);		// 부모가 그룹이면 그룹 내에서 상대적 위치 반영
	void setPositionRelative(const SGVec2& pos);

protected:
	SGUIElementInfo* m_pBaseInfo;
	SGUIGroup* m_pParent;
	SGVec2 m_AbsolutePosition;
	State m_eState;

	bool m_bLoaded;
	bool m_bFocused;

	SGActionFn<SGEventMouse*> m_fnMouseClickCallback;
};


