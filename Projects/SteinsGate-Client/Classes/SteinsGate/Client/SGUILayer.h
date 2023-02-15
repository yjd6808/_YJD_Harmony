/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:09 PM
 * =====================
 * UI 마스타 그룹 외에 절대 addChild 금지
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGUIGroup.h>

class SGUILayer : public SGLayer
{
public:
	SGUILayer();

	static SGUILayer* create();

	bool init() override;
	void onMouseMove(SGEventMouse* mouseEvent);
	void onMouseDown(SGEventMouse* mouseEvent);
	void onMouseUp(SGEventMouse* mouseEvent);
	void onMouseScroll(SGEventMouse* mouseEvent);

	void addUIGroup(int groupCode);
	void removeUIGroup(int groupCode);
	void clear();
	void clearUnload();
private:
	SGUIGroup* m_pPrevOverStateGroup;
	SGUIGroup* m_pPrevPressedStateGroup;
};
