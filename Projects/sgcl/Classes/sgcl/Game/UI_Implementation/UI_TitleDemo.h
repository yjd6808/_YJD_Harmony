/*
 * 작성자: 윤정도
 * =====================
 * WindowStyle:Title 데모 (타이틀바 + 캡션 버튼 + 최소화/최대화)
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

class UI_TitleDemo : public sgui::Window
{
public:
	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;

private:
	sgui::TextBlock* pLabelState_ = nullptr;

	void UpdateStateLabel();
};
