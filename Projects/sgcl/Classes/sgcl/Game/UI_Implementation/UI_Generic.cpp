#include "Game/UI_Implementation/UI_Generic.h"
#include "sgcl/Game/UI/UIButton.h"
#include "sgcl/Game/UI/UICheckBox.h"
#include "sgcl/Game/UI/UIToggleButton.h"
#include "sgcl/Game/UI/UIProgressBar.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "sg/Core/AppConfig.h"
#include "jc/FileSystem/Path.h"
#include "jc/FileSystem/File.h"
#include "2d/CCLabel.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

REGISTER_UI(ui_generic, UI_Generic)

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

UI_Generic::UI_Generic(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pLabelTitle_(nullptr)
, pBtnClose_(nullptr)
, pChkA_(nullptr)
, pChkB_(nullptr)
, pChkC_(nullptr)
, pChkD_(nullptr)
, pToggleOpt_(nullptr)
, pPbar1_(nullptr)
, pPbar2_(nullptr)
, pPbar3_(nullptr)
, pPbar4_(nullptr)
, pPbar5_(nullptr)
, pPbar6_(nullptr)
, pBtnThemeDark_(nullptr)
, pBtnThemeLight_(nullptr)
, pBtnThemeSilver_(nullptr)
, pLabelThemeStatus_(nullptr)
{
}

void UI_Generic::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	pLabelTitle_ = FindElementByName<UILabel>("title");
	pBtnClose_ = FindElementByName<UIButton>("close");
	pChkA_ = FindElementByName<UICheckBox>("chk_a");
	pChkB_ = FindElementByName<UICheckBox>("chk_b");
	pChkC_ = FindElementByName<UICheckBox>("chk_c");
	pChkD_ = FindElementByName<UICheckBox>("chk_d");
	pToggleOpt_ = FindElementByName<UIToggleButton>("toggle_x");
	pPbar1_ = FindElementByName<UIProgressBar>("pbar_1");
	pPbar2_ = FindElementByName<UIProgressBar>("pbar_2");
	pPbar3_ = FindElementByName<UIProgressBar>("pbar_3");
	pPbar4_ = FindElementByName<UIProgressBar>("pbar_4");
	pPbar5_ = FindElementByName<UIProgressBar>("pbar_5");
	pPbar6_ = FindElementByName<UIProgressBar>("pbar_6");

	pBtnThemeDark_ = FindElementByName<UIButton>("btn_theme_dark");
	pBtnThemeLight_ = FindElementByName<UIButton>("btn_theme_light");
	pBtnThemeSilver_ = FindElementByName<UIButton>("btn_theme_silver");
	pLabelThemeStatus_ = FindElementByName<UILabel>("lbl_theme_status");

	_LogDebug_("[UI_Generic] OnInit title=%p close=%p chkA=%p chkB=%p chkC=%p chkD=%p toggle=%p pbar1=%p pbar2=%p pbar3=%p pbar4=%p pbar5=%p pbar6=%p",
		pLabelTitle_, pBtnClose_, pChkA_, pChkB_, pChkC_, pChkD_, pToggleOpt_,
		pPbar1_, pPbar2_, pPbar3_, pPbar4_, pPbar5_, pPbar6_);
	_LogDebug_("[UI_Generic] OnInit themeBtn dark=%p light=%p silver=%p status=%p",
		pBtnThemeDark_, pBtnThemeLight_, pBtnThemeSilver_, pLabelThemeStatus_);
}

void UI_Generic::OnLoaded()
{
	if (auto* p = FindElementByName<UIButton>("btn_small"))  p->SetUISize({ 60, 45 });
	if (auto* p = FindElementByName<UIButton>("btn_medium")) p->SetUISize({ 120, 60 });
	if (auto* p = FindElementByName<UIButton>("btn_large"))  p->SetUISize({ 200, 75 });
	if (auto* p = FindElementByName<UIButton>("btn_wide"))   p->SetUISize({ 200, 45 });
	if (auto* p = FindElementByName<UIButton>("btn_tall"))   p->SetUISize({ 80, 90 });

	if (pChkA_) pChkA_->SetUISize({ 45, 45 });
	if (pChkB_) pChkB_->SetUISize({ 60, 45 });
	if (pChkC_) pChkC_->SetUISize({ 90, 67 });
	if (pChkD_) pChkD_->SetUISize({ 120, 90 });

	if (pToggleOpt_)
	{
		pToggleOpt_->SetUISize({ 54, 28 });
		pToggleOpt_->SetToggleState(ToggleState::eToggled);
	}

	if (pPbar1_) pPbar1_->SetPercent(25.0f);
	if (pPbar2_) pPbar2_->SetPercent(50.0f);
	if (pPbar3_) pPbar3_->SetPercent(75.0f);
	if (pPbar4_) pPbar4_->SetPercent(60.0f);
	if (pPbar5_) pPbar5_->SetPercent(90.0f);
	if (pPbar6_) pPbar6_->SetPercent(40.0f);

	if (pBtnThemeDark_)	pBtnThemeDark_->SetUISize({ 56, 22 });
	if (pBtnThemeLight_) pBtnThemeLight_->SetUISize({ 56, 22 });
	if (pBtnThemeSilver_) pBtnThemeSilver_->SetUISize({ 56, 22 });

	BuildThemeTestButtons();
	UpdateThemeStatusLabel();

	_LogDebug_("[UI_Generic] ROOT this=%p pos=(%.0f,%.0f) contentSize=(%.0f,%.0f) uiSize=(%.0f,%.0f) scale=(%.2f,%.2f) loaded=%d",
		this, getPositionX(), getPositionY(),
		getContentSize().width, getContentSize().height,
		uiSize_.width, uiSize_.height,
		getScaleX(), getScaleY(), isLoaded_ ? 1 : 0);

	const char* dumpNames[] = { "title", "close", "btn_small", "btn_medium", "btn_large", "btn_wide", "btn_tall",
		"chk_a", "chk_b", "chk_c", "chk_d", "toggle_x",
		"pbar_1", "pbar_2", "pbar_3", "pbar_4", "pbar_5", "pbar_6" };
	for (const char* n : dumpNames)
	{
		UIElement* p = UIGroup::FindElementByName(n);
		if (!p) { _LogDebug_("[UI_Generic] LAYOUT name=%s NOT-FOUND", n); continue; }
		_LogDebug_("[UI_Generic] LAYOUT name=%s el=%p parent=%p root=%p pos=(%.0f,%.0f) size=(%.0f,%.0f) abs=(%.0f,%.0f) visible=%d",
			n, p, p->getParent(), this,
			p->getPositionX(), p->getPositionY(),
			p->GetUISize().width, p->GetUISize().height,
			p->GetAbsolutePosition().x, p->GetAbsolutePosition().y,
			p->isVisible() ? 1 : 0);
	}
}

void UI_Generic::OnUpdate(float _dt)
{
	UNUSED(_dt);
}

void UI_Generic::OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
	const char* name = _pElement->GetName();

	if (strcmp(name, "close") == 0)
	{
		return;
	}

	if (strcmp(name, "btn_small") == 0)   { _LogDebug_("[UI_Generic] btn_small clicked"); }
	else if (strcmp(name, "btn_medium") == 0) { _LogDebug_("[UI_Generic] btn_medium clicked"); }
	else if (strcmp(name, "btn_large") == 0)  { _LogDebug_("[UI_Generic] btn_large clicked"); }
	else if (strcmp(name, "btn_wide") == 0)   { _LogDebug_("[UI_Generic] btn_wide clicked"); }
	else if (strcmp(name, "btn_tall") == 0)   { _LogDebug_("[UI_Generic] btn_tall clicked"); }
	else if (strcmp(name, "chk_a") == 0)      { _LogDebug_("[UI_Generic] chk_a toggled: checked=%d", pChkA_->IsChecked()); }
	else if (strcmp(name, "chk_b") == 0)      { _LogDebug_("[UI_Generic] chk_b toggled: checked=%d", pChkB_->IsChecked()); }
	else if (strcmp(name, "chk_c") == 0)      { _LogDebug_("[UI_Generic] chk_c toggled: checked=%d", pChkC_->IsChecked()); }
	else if (strcmp(name, "chk_d") == 0)      { _LogDebug_("[UI_Generic] chk_d toggled: checked=%d", pChkD_->IsChecked()); }
	else if (strcmp(name, "toggle_x") == 0)   { _LogDebug_("[UI_Generic] toggle_x toggled"); }

	else if (strcmp(name, "btn_theme_dark") == 0)
	{
		ApplyThemeFile("modern-dark-gold.json", UIColorScheme::Dark);
	}
	else if (strcmp(name, "btn_theme_light") == 0)
	{
		ApplyThemeFile("modern-dark-gold.json", UIColorScheme::Light);
	}
	else if (strcmp(name, "btn_theme_silver") == 0)
	{
		ApplyThemeFile("aurora-light-silver.json", UIColorScheme::Dark);
	}
	else if (strcmp(name, "tbtn_scheme_dark") == 0)
	{
		const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "dark");
		ApplyThemeFile(json.Source(), UIColorScheme::Dark);
	}
	else if (strcmp(name, "tbtn_scheme_light") == 0)
	{
		const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "light");
		ApplyThemeFile(json.Source(), UIColorScheme::Light);
	}
	else if (strcmp(name, "tbtn_scheme_system") == 0)
	{
		const jc::String json = ResolveThemeJsonPath(pLastThemeId_, "dark");
		ApplyThemeFile(json.Source(), UIColorScheme::System);
	}
	else if (strncmp(name, "tbtn_", 5) == 0)
	{
		const int index = atoi(name + 5);
		if (index >= 0 && index < kThemeTestCount)
		{
			pLastThemeId_ = kThemeTestIds[index];

			UIThemeManager* pMgr = UIThemeManager::Get();
			const UIColorScheme scheme = pMgr->GetActiveScheme();
			const char* mode = scheme == UIColorScheme::Light ? "light" : "dark";
			const jc::String json = ResolveThemeJsonPath(pLastThemeId_, mode);
			_LogDebug_("[UI_Generic] theme button: %s -> %s (scheme=%d)", name, json.Source(), (int)scheme);
			ApplyThemeFile(json.Source(), scheme);
		}
	}
}

void UI_Generic::BuildThemeTestButtons()
{
	constexpr float kBtnH = 18.0f;
	constexpr float kSchemeX[] = { 660.0f, 718.0f, 776.0f };
	const char* kSchemeNames[] = { "tbtn_scheme_dark", "tbtn_scheme_light", "tbtn_scheme_system" };
	const char* kSchemeLabels[] = { "Dark", "Light", "System" };
	constexpr int kSchemeCount = 3;
	constexpr int kPerCol = 20;

	for (int i = 0; i < kSchemeCount; ++i)
		CreateTestButton(kSchemeNames[i], kSchemeLabels[i], kSchemeX[i], 452.0f, 54.0f, kBtnH);

	for (int i = 0; i < kThemeTestCount; ++i)
	{
		const int col = i / kPerCol;
		const int row = i % kPerCol;
		const float x = col == 0 ? 660.0f : 770.0f;
		const float y = 428.0f - row * 22.0f;
		char name[64];
		snprintf(name, sizeof(name), "tbtn_%d", i);
		CreateTestButton(name, kThemeTestIds[i], x, y, 100.0f, kBtnH);
	}
}

UIButton* UI_Generic::CreateTestButton(const char* _name, const char* _label, float _x, float _y, float _w, float _h)
{
	UIButtonInfo* pInfo = dbg_new UIButtonInfo();
	pInfo->type_ = UIElementType::Button;
	strcpy_s(pInfo->name_, _name);

	UIButton* pBtn = UIButton::Create(GetRootGroup(), this, pInfo, true);
	pBtn->SetUISize({ _w, _h });
	pBtn->SetRelativePosition(_x, _y);
	AddChild(pBtn);
	pBtn->Load();

	auto* pLabel = Label::createWithSystemFont(_label, "Arial", 9);
	pLabel->setAnchorPoint(Vec2(0.0f, 0.5f));
	pLabel->setPosition(Vec2(4.0f, _h * 0.5f));
	pBtn->addChild(pLabel);

	return pBtn;
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

	pLabelThemeStatus_->setText(jc::StringUtil::Format("%s (%s)", pThemeLabel, isDark ? "Dark" : "Light").Source());
	_LogDebug_("[UI_Generic] ThemeStatus updated: %s (%s)", pThemeLabel, isDark ? "Dark" : "Light");
}
