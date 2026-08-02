/*
 * 작성자: 윤정도
 * =====================
 * 테마/스킴 전환 테스트 (테마 JSON 전환 + 컨트롤 갤러리 + 타이틀 윈도우 border 확인)
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_ThemeTest.h"

#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/UI/Host/UIManager.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "sg/Core/AppConfig.h"
#include "jc/FileSystem/Path.h"
#include "jc/FileSystem/File.h"

#include <cstdio>

USING_NS_CC;
USING_NS_JC;

using namespace sgui;

namespace
{
// JSON 파일 없으면 코드 폴백 (마지막 선택 유지)
static jc::String ResolveThemeJsonPath(const char* _id, const char* _mode)
{
	const jc::String baseDir = jc::Path::Combine(g_cAppConfig.resDataPath_, "theme");
	const jc::String schemeJson = jc::StringUtil::Format("%s.%s.json", _id, _mode);
	if (jc::File::Exist(jc::Path::Combine(baseDir, schemeJson)))
	{
		return schemeJson;
	}

	return jc::StringUtil::Format("%s.json", _id);
}
} // namespace

void UI_ThemeTest::OnInit(const CDataMap<>& _param)
{
	Window::OnInit(_param);

	if (IsXmlLoaded())
	{
		pLabelStatus_ = FindName<sgui::TextBlock>("lbl_status");

		// 스킴 전환
		if (sgui::Button* pBtn = FindName<sgui::Button>("btn_scheme_dark"))
		{
			pBtn->SetClickCallback([this](sgui::ButtonBase*)
			{
				const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "dark");
				ApplyThemeFile(json.Source(), UIColorScheme::Dark);
			});
		}

		if (sgui::Button* pBtn = FindName<sgui::Button>("btn_scheme_light"))
		{
			pBtn->SetClickCallback([this](sgui::ButtonBase*)
			{
				const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "light");
				ApplyThemeFile(json.Source(), UIColorScheme::Light);
			});
		}

		// 테마 전환 (버튼별 테마 id)
		struct ThemeButtonSpec
		{
			const char* name;
			const char* themeId;
		};

		const ThemeButtonSpec specs[] = {
			{ "btn_theme_default", "engine-default" },
			{ "btn_theme_gold", "modern-dark-gold" },
			{ "btn_theme_amoled", "amoled" },
			{ "btn_theme_dracula", "dracula" },
			{ "btn_theme_aurora", "aurora-light-silver" },
			{ "btn_theme_github", "github" },
		};

		for (const auto& spec : specs)
		{
			if (sgui::Button* pBtn = FindName<sgui::Button>(spec.name))
			{
				pBtn->SetClickCallback([this, spec](sgui::ButtonBase*)
				{
					pLastThemeId_ = spec.themeId;

					UIThemeManager* pMgr = UIThemeManager::Get();
					const UIColorScheme scheme = pMgr->GetActiveScheme();
					const char* mode = scheme == UIColorScheme::Light ? "light" : "dark";
					const jc::String json = ResolveThemeJsonPath(pLastThemeId_, mode);
					_LogDebug_("[UI_ThemeTest] theme -> %s (scheme=%d) json=%s", spec.themeId, (int)scheme, json.Source());
					ApplyThemeFile(json.Source(), scheme);
				});
			}
		}

		if (sgui::Button* pBtn = FindName<sgui::Button>("btn_open_title"))
		{
			pBtn->SetClickCallback([](sgui::ButtonBase*) { g_cUIMgr.Show("ui_title_demo"); });
		}

		if (sgui::Button* pBtn = FindName<sgui::Button>("btn_close"))
		{
			pBtn->SetClickCallback([this](sgui::ButtonBase*) { Close(); });
		}

		UpdateStatusLabel();
	}
}

void UI_ThemeTest::OnLoaded()
{
	UpdateStatusLabel();
}

void UI_ThemeTest::ApplyThemeFile(const char* _jsonName, UIColorScheme _scheme)
{
	const jc::String jsonPath = jc::Path::Combine(jc::Path::Combine(g_cAppConfig.resDataPath_, "theme"), _jsonName);
	_LogDebug_("[UI_ThemeTest] ApplyThemeFile path=%s scheme=%d", jsonPath.Source(), (int)_scheme);
	if (UIThemeManager::Get()->ApplyTheme(jsonPath.Source(), _scheme))
	{
		UpdateStatusLabel();
	}
}

void UI_ThemeTest::UpdateStatusLabel()
{
	if (!pLabelStatus_)
	{
		return;
	}

	UIThemeManager* pMgr = UIThemeManager::Get();
	const bool isDark = pMgr->GetActiveScheme() != UIColorScheme::Light;
	const char* pThemeName = pMgr->GetActiveThemeName();
	const char* pThemeLabel = (pThemeName && pThemeName[0]) ? pThemeName : "(default)";

	pLabelStatus_->SetText(jc::StringUtil::Format("%s (%s)", pThemeLabel, isDark ? "Dark" : "Light").Source());
	_LogDebug_("[UI_ThemeTest] ThemeStatus: %s (%s)", pThemeLabel, isDark ? "Dark" : "Light");
}

REGISTER_WINDOW(UI_ThemeTest, "ui_theme_test")
