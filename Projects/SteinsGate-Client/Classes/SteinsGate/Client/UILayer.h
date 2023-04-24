/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:09 PM
 * =====================
 * UI 마스타 그룹 외에 절대 addChild 금지
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/UIGroup.h>

class UILayer : public SGLayer
{
public:
	UILayer();

	static UILayer* create();

	bool init() override;
	void onMouseMove(SGEventMouse* mouseEvent);
	void onMouseDown(SGEventMouse* mouseEvent);
	void onMouseUp(SGEventMouse* mouseEvent);
	void onMouseScroll(SGEventMouse* mouseEvent) const;

	void update(float dt) override;
	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

	UIMasterGroup* findGroup(int groupCode);
	void forEach(const SGActionFn<UIMasterGroup*>& actionFn);
	void addUIGroup(int groupCode);
	void removeUIGroup(int groupCode);
	void clear();
	void clearUnload();
private:
	UIGroup* m_pPrevOverStateGroup;
	UIGroup* m_pPrevPressedStateGroup;
};
