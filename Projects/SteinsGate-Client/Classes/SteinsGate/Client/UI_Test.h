/*
 * 작성자: 윤정도
 * 생성일: 4/27/2023 10:50:56 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIRootGroup.h>

class UI_Test : public UIRootGroup
{
public:
	enum ScaleState
	{
		eIncrease,
		eDecrease
	};

	UI_Test(UIGroupInfo* _pGroupInfo);

protected:
	void OnInit() override;
	void OnLoaded() override;
	void OnUpdate(float _dt) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;
	void OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent) override;

private:
	UIGroup* pGroup01_;
	UIGroup* pGroup02_;

	SGSize groupO1DefaultSize_;
	SGSize groupO2DefaultSize_;

	int fontCode_;
	SGVector<SGLabel*> testLabelList_;
	bool testFont_;

	float scale_;
	ScaleState scaleState_;
};
