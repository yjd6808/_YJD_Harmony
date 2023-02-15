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


	bool loaded();

	virtual void restoreState(State state);
	virtual void load();
	virtual void unload();
	virtual bool onMouseMove(SGEventMouse* mouseEvent);
	virtual bool onMouseDown(SGEventMouse* mouseEvent);
	virtual bool onMouseUp(SGEventMouse* mouseEvent);
	virtual bool onMouseScroll(SGEventMouse* mouseEvent);
	virtual int getCode() = 0;
	virtual UIElementType_t getElementType() = 0;

	SGRect getWorldBoundingBox();
	void setEnabled(bool enabled);
	

	void setMouseClick(const SGActionFn<SGEventMouse*>& callback) {
		m_fnMouseClickCallback = callback;
	}
protected:
	bool m_bLoaded;
	SGUIGroup* m_pParent;
	SGActionFn<SGEventMouse*> m_fnMouseClickCallback;
	State m_eState;
};


