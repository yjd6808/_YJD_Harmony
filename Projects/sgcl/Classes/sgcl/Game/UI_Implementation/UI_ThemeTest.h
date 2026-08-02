/*
 * 작성자: 윤정도
 * =====================
 * 테마/스킴 전환 테스트 (테마 JSON 전환 + 컨트롤 갤러리 + 타이틀 윈도우 border 확인)
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

class UI_ThemeTest : public sgui::Window
{
public:
	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;

private:
	sgui::TextBlock* pLabelStatus_ = nullptr;
	const char* pLastThemeId_ = "engine-default";

	void ApplyThemeFile(const char* _jsonName, UIColorScheme _scheme);
	void UpdateStatusLabel();
};
