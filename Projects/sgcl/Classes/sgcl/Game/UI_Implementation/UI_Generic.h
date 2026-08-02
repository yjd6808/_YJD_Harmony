/*
 * 작성자: 윤정도
 * =====================
 * 컨트롤/테마 테스트 화면 (sgui 기반 재구현)
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

class UI_Generic : public sgui::Window
{
public:
	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;

private:
	sgui::TextBlock* pLabelTitle_ = nullptr;
	sgui::Button* pBtnClose_ = nullptr;
	sgui::CheckBox* pChkA_ = nullptr;
	sgui::CheckBox* pChkB_ = nullptr;
	sgui::CheckBox* pChkC_ = nullptr;
	sgui::CheckBox* pChkD_ = nullptr;
	sgui::ToggleButton* pToggleOpt_ = nullptr;
	sgui::ProgressBar* pPbar1_ = nullptr;
	sgui::ProgressBar* pPbar2_ = nullptr;
	sgui::ProgressBar* pPbar3_ = nullptr;
	sgui::ProgressBar* pPbar4_ = nullptr;
	sgui::ProgressBar* pPbar5_ = nullptr;
	sgui::ProgressBar* pPbar6_ = nullptr;
	sgui::Slider* pSlider_ = nullptr;
	sgui::TextBlock* pLabelThemeStatus_ = nullptr;

	const char* pLastThemeId_ = "modern-dark-gold";

	void ApplyThemeFile(const char* _jsonName, UIColorScheme _scheme);
	void UpdateThemeStatusLabel();
	sgui::Panel* BuildControlShowcase();
	sgui::Panel* BuildThemeTestButtons();
};
