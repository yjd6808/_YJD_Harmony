#pragma once

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/UIRegister.h"

class UI_Generic : public UIRootGroup
{
public:
	UI_Generic(UIGroupInfo* _pGroupInfo);

protected:
	void OnInit(const jc::CDataMap<>& _param) override;
	void OnLoaded() override;
	void OnUpdate(float _dt) override;
	void OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent) override;

private:
	UILabel* pLabelTitle_;
	UIButton* pBtnClose_;
	UICheckBox* pChkA_;
	UICheckBox* pChkB_;
	UICheckBox* pChkC_;
	UICheckBox* pChkD_;
	UIToggleButton* pToggleOpt_;
	UIProgressBar* pPbar1_;
	UIProgressBar* pPbar2_;
	UIProgressBar* pPbar3_;
	UIProgressBar* pPbar4_;
	UIProgressBar* pPbar5_;
	UIProgressBar* pPbar6_;
};
