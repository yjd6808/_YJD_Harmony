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
    UILabel* pLabelContent_;
    UIButton* pBtnClose_;
    UICheckBox* pChkOpt1_;
    UICheckBox* pChkOpt2_;
    UIEditBox* pEditInput_;
};
