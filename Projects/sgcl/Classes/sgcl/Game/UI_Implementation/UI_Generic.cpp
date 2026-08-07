/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Generic.h"

#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "sg/Core/AppConfig.h"
#include "jc/FileSystem/Path.h"
#include "jc/FileSystem/File.h"

#include <cstdio>

USING_NS_CC;
USING_NS_JC;

using namespace sgui;

static const char* kThemeTestIds[] = {
	"modern-dark-gold", "aurora-light-silver",
	"amoled", "aura", "ayu", "carbonfox", "catppuccin-frappe", "catppuccin-macchiato",
	"catppuccin", "cobalt2", "cursor", "dracula", "everforest", "flexoki", "github",
	"gruvbox", "kanagawa", "lucent-orng", "material", "matrix", "mercury", "monokai",
	"nightowl", "nord", "oc-2", "one-dark", "onedarkpro", "opencode", "orng",
	"osaka-jade", "palenight", "rosepine", "shadesofpurple", "solarized", "synthwave84",
	"tokyonight", "vercel", "vesper", "zenburn"
};
static constexpr int kThemeTestCount = (int)(sizeof(kThemeTestIds) / sizeof(kThemeTestIds[0]));

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

void UI_Generic::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	// ==================== XML 우선 (layout/ui_generic.xml 병합 시) ====================
	// sg-main 방식: XML로 로드된 Element를 OnInit 단에서 FindElementByName으로 멤버에 할당한다.
	if (IsXmlLoaded())
	{
		pLabelTitle_ = FindElementByName<TextBlock>("title");
		pBtnClose_ = FindElementByName<Button>("close");
		pChkA_ = FindElementByName<CheckBox>("chk_a");
		pChkB_ = FindElementByName<CheckBox>("chk_b");
		pChkC_ = FindElementByName<CheckBox>("chk_c");
		pChkD_ = FindElementByName<CheckBox>("chk_d");
		pToggleOpt_ = FindElementByName<ToggleButton>("toggle_x");
		pPbar1_ = FindElementByName<ProgressBar>("pbar_1");
		pPbar2_ = FindElementByName<ProgressBar>("pbar_2");
		pPbar3_ = FindElementByName<ProgressBar>("pbar_3");
		pPbar4_ = FindElementByName<ProgressBar>("pbar_4");
		pPbar5_ = FindElementByName<ProgressBar>("pbar_5");
		pPbar6_ = FindElementByName<ProgressBar>("pbar_6");
		pLabelThemeStatus_ = FindElementByName<TextBlock>("lbl_theme_status");

		if (pBtnClose_)
		{
			pBtnClose_->SetClickCallback([this](ButtonBase*) { Close(); });
		}

		const auto logChecked = [](UIElement* _pSender, UIEvent* _pEvent)
		{
			UIEventChecked* pChecked = _pEvent->Cast<UIEventChecked>();
			_LogDebug_("[UI_Generic] %s toggled: checked=%d",
				static_cast<cc::Node*>(_pSender)->getName().c_str(), pChecked->checked_ ? 1 : 0);
		};

		CheckBox* pChecks[] = { pChkA_, pChkB_, pChkC_, pChkD_ };
		for (CheckBox* pChk : pChecks)
		{
			if (pChk)
			{
				pChk->AddHandler(ueCheckedChanged, logChecked);
			}
		}

		if (pToggleOpt_)
		{
			pToggleOpt_->AddHandler(ueCheckedChanged, [](UIElement*, UIEvent*) { _LogDebug_("[UI_Generic] toggle_x toggled"); });
		}

		// 프로그레스바 초기 퍼센트 (sg-main 로직 유지)
		if (pPbar1_)
		{
			pPbar1_->SetPercent(25.0f);
		}
		if (pPbar2_)
		{
			pPbar2_->SetPercent(50.0f);
		}
		if (pPbar3_)
		{
			pPbar3_->SetPercent(75.0f);
		}
		if (pPbar4_)
		{
			pPbar4_->SetPercent(60.0f);
		}
		if (pPbar5_)
		{
			pPbar5_->SetPercent(90.0f);
		}
		if (pPbar6_)
		{
			pPbar6_->SetPercent(40.0f);
		}

		_LogDebug_("[UI_Generic] OnInit(XML) title=%p close=%p chk=%p,%p,%p,%p toggle=%p pbar1=%p pbar6=%p status=%p",
			pLabelTitle_, pBtnClose_, pChkA_, pChkB_, pChkC_, pChkD_,
			pToggleOpt_, pPbar1_, pPbar6_, pLabelThemeStatus_);

		if (Button* pBtnSmall = FindElementByName<Button>("btn_small"))
		{
			Button* pBtnMedium = FindElementByName<Button>("btn_medium");
			Button* pBtnLarge = FindElementByName<Button>("btn_large");
			_LogDebug_("[UI_Generic] expr btn_small=%.1fx%.1f btn_medium=%.1fx%.1f btn_large=%.1fx%.1f",
				pBtnSmall->GetWidth(), pBtnSmall->GetHeight(),
				pBtnMedium ? pBtnMedium->GetWidth() : 0.0f, pBtnMedium ? pBtnMedium->GetHeight() : 0.0f,
				pBtnLarge ? pBtnLarge->GetWidth() : 0.0f, pBtnLarge ? pBtnLarge->GetHeight() : 0.0f);
		}
		if (pChkA_)
		{
			_LogDebug_("[UI_Generic] expr chk_a=%.1fx%.1f", pChkA_->GetWidth(), pChkA_->GetHeight());
		}
		if (pPbar1_)
		{
			_LogDebug_("[UI_Generic] expr pbar_1=%.1fx%.1f", pPbar1_->GetWidth(), pPbar1_->GetHeight());
		}
		if (pLabelTitle_)
		{
			_LogDebug_("[UI_Generic] expr title_text=%s", pLabelTitle_->GetText().Source());
		}
		return;
	}

	// ==================== 코드 빌드 폴백 (XML 없음) ====================
	// 좌측: 컨트롤 쇼케이스 / 우측: 테마 테스트 버튼
	Grid* pRootGrid = Grid::Create();
	pRootGrid->AddColumnDefinition(GridLength::Star(1.0f));
	pRootGrid->AddColumnDefinition(GridLength::Auto());
	pRootGrid->AddRowDefinition(GridLength::Star(1.0f));
	pRootGrid->SetMargin(Thickness(12.0f));

	Panel* pShowcase = BuildControlShowcase();
	pShowcase->SetGridColumn(0);
	pShowcase->SetGridRow(0);
	pRootGrid->AddChild(pShowcase);

	Panel* pThemePanel = BuildThemeTestButtons();
	pThemePanel->SetGridColumn(1);
	pThemePanel->SetGridRow(0);
	pRootGrid->AddChild(pThemePanel);

	AddChild(pRootGrid);
}

Panel* UI_Generic::BuildControlShowcase()
{
	StackPanel* pStack = StackPanel::Create(Orientation::Vertical);
	pStack->SetSpacing(8.0f);
	pStack->SetHorizontalAlignment(HorizontalAlignment::Left);

	// 타이틀 + 닫기
	DockPanel* pTitleRow = DockPanel::Create();
	pTitleRow->SetLastChildFill(true);

	pBtnClose_ = Button::Create("X");
	pBtnClose_->setName("close");
	pBtnClose_->SetClickCallback([this](ButtonBase*) { Close(); });
	pBtnClose_->SetDock(Dock::Right);
	pTitleRow->AddChild(pBtnClose_);

	pLabelTitle_ = TextBlock::Create("Generic Control Showcase");
	pLabelTitle_->setName("title");
	pTitleRow->AddChild(pLabelTitle_);

	pStack->AddChild(pTitleRow);

	// 버튼 크기 테스트
	StackPanel* pButtonRow = StackPanel::Create(Orientation::Horizontal);
	pButtonRow->SetSpacing(6.0f);

	struct { const char* name; const char* label; float w; float h; } buttonSpecs[] = {
		{ "btn_small", "Small", 60.0f, 45.0f },
		{ "btn_medium", "Medium", 120.0f, 60.0f },
		{ "btn_large", "Large", 200.0f, 75.0f },
		{ "btn_wide", "Wide", 200.0f, 45.0f },
		{ "btn_tall", "Tall", 80.0f, 90.0f },
	};

	for (const auto& spec : buttonSpecs)
	{
		Button* pBtn = Button::Create(spec.label);
		pBtn->setName(spec.name);
		pBtn->SetWidth(spec.w);
		pBtn->SetHeight(spec.h);
		pBtn->SetClickCallback([](ButtonBase* _pSender) { _LogDebug_("[UI_Generic] %s clicked", _pSender->getName().c_str()); });
		pButtonRow->AddChild(pBtn);
	}

	pStack->AddChild(pButtonRow);

	// 체크박스 / 토글
	StackPanel* pCheckRow = StackPanel::Create(Orientation::Horizontal);
	pCheckRow->SetSpacing(10.0f);

	const auto logChecked = [](UIElement* _pSender, UIEvent* _pEvent)
	{
		UIEventChecked* pChecked = _pEvent->Cast<UIEventChecked>();
		_LogDebug_("[UI_Generic] %s toggled: checked=%d",
			static_cast<cc::Node*>(_pSender)->getName().c_str(), pChecked->checked_ ? 1 : 0);
	};

	pChkA_ = CheckBox::Create("A");
	pChkA_->setName("chk_a");
	pChkA_->AddHandler(ueCheckedChanged, logChecked);
	pCheckRow->AddChild(pChkA_);

	pChkB_ = CheckBox::Create("B");
	pChkB_->setName("chk_b");
	pChkB_->AddHandler(ueCheckedChanged, logChecked);
	pCheckRow->AddChild(pChkB_);

	pChkC_ = CheckBox::Create("C");
	pChkC_->setName("chk_c");
	pChkC_->AddHandler(ueCheckedChanged, logChecked);
	pCheckRow->AddChild(pChkC_);

	pChkD_ = CheckBox::Create("D");
	pChkD_->setName("chk_d");
	pChkD_->AddHandler(ueCheckedChanged, logChecked);
	pCheckRow->AddChild(pChkD_);

	pToggleOpt_ = ToggleButton::Create(nullptr);
	pToggleOpt_->setName("toggle_x");
	pToggleOpt_->BuildSwitchVisuals();
	pToggleOpt_->SetWidth(54.0f);
	pToggleOpt_->SetHeight(28.0f);
	pToggleOpt_->SetIsChecked(true);
	pToggleOpt_->AddHandler(ueCheckedChanged, [](UIElement*, UIEvent*) { _LogDebug_("[UI_Generic] toggle_x toggled"); });
	pCheckRow->AddChild(pToggleOpt_);

	pStack->AddChild(pCheckRow);

	// 프로그레스바
	struct { const char* name; float percent; sgui::ProgressBar** target; } pbarSpecs[] = {
		{ "pbar_1", 25.0f, &pPbar1_ },
		{ "pbar_2", 50.0f, &pPbar2_ },
		{ "pbar_3", 75.0f, &pPbar3_ },
		{ "pbar_4", 60.0f, &pPbar4_ },
		{ "pbar_5", 90.0f, &pPbar5_ },
		{ "pbar_6", 40.0f, &pPbar6_ },
	};

	for (const auto& spec : pbarSpecs)
	{
		ProgressBar* pBar = ProgressBar::Create();
		pBar->setName(spec.name);
		pBar->SetWidth(220.0f);
		pBar->SetHeight(14.0f);
		pBar->SetPercent(spec.percent);
		*spec.target = pBar;
		pStack->AddChild(pBar);
	}

	// 슬라이더
	pSlider_ = Slider::Create();
	pSlider_->setName("slider_x");
	pSlider_->SetWidth(220.0f);
	pSlider_->SetValue(50.0);
	pStack->AddChild(pSlider_);

	return pStack;
}

Panel* UI_Generic::BuildThemeTestButtons()
{
	StackPanel* pThemeStack = StackPanel::Create(Orientation::Vertical);
	pThemeStack->SetSpacing(4.0f);
	pThemeStack->SetMargin(Thickness(16.0f, 0.0f, 0.0f, 0.0f));

	pLabelThemeStatus_ = TextBlock::Create("(theme)");
	pLabelThemeStatus_->setName("lbl_theme_status");
	pThemeStack->AddChild(pLabelThemeStatus_);

	// 스키마 전환 버튼
	StackPanel* pSchemeRow = StackPanel::Create(Orientation::Horizontal);
	pSchemeRow->SetSpacing(4.0f);

	Button* pBtnSchemeDark = Button::Create("Dark");
	pBtnSchemeDark->setName("tbtn_scheme_dark");
	pBtnSchemeDark->SetClickCallback([this](ButtonBase*)
	{
		const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "dark");
		ApplyThemeFile(json.Source(), UIColorScheme::Dark);
	});
	pSchemeRow->AddChild(pBtnSchemeDark);

	Button* pBtnSchemeLight = Button::Create("Light");
	pBtnSchemeLight->setName("tbtn_scheme_light");
	pBtnSchemeLight->SetClickCallback([this](ButtonBase*)
	{
		const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "light");
		ApplyThemeFile(json.Source(), UIColorScheme::Light);
	});
	pSchemeRow->AddChild(pBtnSchemeLight);

	Button* pBtnSchemeSystem = Button::Create("System");
	pBtnSchemeSystem->setName("tbtn_scheme_system");
	pBtnSchemeSystem->SetClickCallback([this](ButtonBase*)
	{
		const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "dark");
		ApplyThemeFile(json.Source(), UIColorScheme::System);
	});
	pSchemeRow->AddChild(pBtnSchemeSystem);

	pThemeStack->AddChild(pSchemeRow);

	// 테마 목록 버튼 (2열 래핑)
	UniformGrid* pThemeGrid = UniformGrid::Create((kThemeTestCount + 1) / 2, 2);
	pThemeGrid->setName("theme_buttons");

	for (int idx = 0; idx < kThemeTestCount; ++idx)
	{
		char name[64];
		snprintf(name, sizeof(name), "tbtn_%d", idx);

		Button* pBtn = Button::Create(kThemeTestIds[idx]);
		pBtn->setName(name);
		pBtn->SetPadding(Thickness(4.0f, 2.0f, 4.0f, 2.0f));
		pBtn->SetMargin(Thickness(1.0f));

		const int index = idx;
		pBtn->SetClickCallback([this, index](ButtonBase*)
		{
			pLastThemeId_ = kThemeTestIds[index];

			UIThemeManager* pMgr = UIThemeManager::Get();
			const UIColorScheme scheme = pMgr->GetActiveScheme();
			const char* mode = scheme == UIColorScheme::Light ? "light" : "dark";
			const jc::String json = ResolveThemeJsonPath(pLastThemeId_, mode);
			_LogDebug_("[UI_Generic] theme button: tbtn_%d -> %s (scheme=%d)", index, json.Source(), (int)scheme);
			ApplyThemeFile(json.Source(), scheme);
		});

		pThemeGrid->AddChild(pBtn);
	}

	pThemeStack->AddChild(pThemeGrid);
	return pThemeStack;
}

void UI_Generic::OnLoaded()
{
	UpdateThemeStatusLabel();
}

void UI_Generic::ApplyThemeFile(const char* _jsonName, UIColorScheme _scheme)
{
	const jc::String jsonPath = jc::Path::Combine(jc::Path::Combine(g_cAppConfig.resDataPath_, "theme"), _jsonName);
	_LogDebug_("[UI_Generic] ApplyThemeFile path=%s scheme=%d", jsonPath.Source(), (int)_scheme);
	if (UIThemeManager::Get()->ApplyTheme(jsonPath.Source(), _scheme))
	{
		UpdateThemeStatusLabel();
	}
}

void UI_Generic::UpdateThemeStatusLabel()
{
	if (!pLabelThemeStatus_)
	{
		return;
	}

	UIThemeManager* pMgr = UIThemeManager::Get();
	const bool isDark = pMgr->GetActiveScheme() == UIColorScheme::Dark;
	const char* pThemeName = pMgr->GetActiveThemeName();
	const char* pThemeLabel = (pThemeName && pThemeName[0]) ? pThemeName : "(default)";

	pLabelThemeStatus_->SetText(jc::StringUtil::Format("%s (%s)", pThemeLabel, isDark ? "Dark" : "Light").Source());
	_LogDebug_("[UI_Generic] ThemeStatus updated: %s (%s)", pThemeLabel, isDark ? "Dark" : "Light");
}

REGISTER_WINDOW(UI_Generic, "ui_generic")
