#pragma once

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/UIRegister.h"

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
	void OnInit(const jc::CDataMap<>& _param) override;
	void OnLoaded() override;
	void OnUpdate(float _dt) override;
	void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state) override;
	void OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent) override;

private:
	UIGroup* pGroup01_;
	UIGroup* pGroup02_;

	cc::size groupO1DefaultSize_;
	cc::size groupO2DefaultSize_;

	int fontCode_;
	jc::Vector<cc::Label*> testLabelList_;
	bool testFont_;

	float scale_;
	ScaleState scaleState_;
};

REGISTER_UI(Test, UI_Test)