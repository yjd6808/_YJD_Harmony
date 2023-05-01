/*
 * 작성자: 윤정도
 * 생성일: 4/27/2023 10:50:56 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/UIMasterGroup.h>

class UI_Test : public UIMasterGroup
{
public:
	enum ScaleState
	{
		eIncrease,
		eDecrease
	};

	UI_Test(UIGroupInfo* groupInfo);

protected:
	void onInit() override;
	void onLoaded() override;
	void onUpdate(float dt) override;
	void onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) override;
	void onMouseUp(UIElement* element, SGEventMouse* mouseEvent) override;
private:
	UIGroup* m_pGroupO1;
	UIGroup* m_pGroupO2;

	// ========================================================
	SGSize m_GroupO1DefaultSize;
	SGSize m_GroupO2DefaultSize;

	int m_iFontCode;
	SGVector<SGLabel*> m_vTestLabelList;

	float m_fScale;
	ScaleState m_eScaleState;
};
