/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:56:57 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

#include <SteinsGate/Client/SGUIInfo.h>
#include <SteinsGate/Client/SGUIDefine.h>
#include <SteinsGate/Client/SGFrameTexture.h>

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

	SGUIElement(SGUIGroup* parent);
	~SGUIElement() override = default;

	bool loaded();

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

	SGRect getWorldBoundingBox();
	void updateState();

	void setCallbackClick(const SGActionFn<SGEventMouse*>& callback);
protected:
	SGUIGroup* m_pParent;
	State m_eState;

	bool m_bLoaded;
	bool m_bFocused;

	SGActionFn<SGEventMouse*> m_fnMouseClickCallback;
};


