/*
 * 작성자: 윤정도
 * =====================
 * 테마 미리보기 카탈로그 (sgui 컨트롤 기반 재구현)
 * UIThemeStudio에서 사용하는 Create/SetPreviewSize/RefreshAll API는 기존과 동일하다.
 */

#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Sgui.h"

class UIThemePreviewCatalog : public cc::Node
{
public:
	static UIThemePreviewCatalog* Create(float _width, float _height);

	void SetPreviewSize(float _width, float _height);
	void RefreshAll();

private:
	sgui::Button* pBtnNormal_ = nullptr;
	sgui::Button* pBtnHover_ = nullptr;
	sgui::Button* pBtnPressed_ = nullptr;
	sgui::Button* pBtnDisabled_ = nullptr;

	sgui::CheckBox* pCheckOff_ = nullptr;
	sgui::CheckBox* pCheckOn_ = nullptr;
	sgui::CheckBox* pCheckDisabled_ = nullptr;

	sgui::ToggleButton* pToggleOff_ = nullptr;
	sgui::ToggleButton* pToggleOn_ = nullptr;
	sgui::ToggleButton* pToggleDisabled_ = nullptr;

	sgui::ProgressBar* pProgressNeutral_ = nullptr;
	sgui::ProgressBar* pProgressSuccess_ = nullptr;
	sgui::ProgressBar* pProgressWarning_ = nullptr;

	void CreateButtons();
	void CreateCheckBoxes();
	void CreateToggles();
	void CreateProgressBars();

	void PlaceElement(sgui::UIElement* _pElement, float _x, float _y, float _w, float _h);
};
