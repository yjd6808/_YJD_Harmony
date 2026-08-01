#include "Game/UI_Implementation/UI_Generic.h"
#include "sgcl/Game/UI/UICheckBox.h"
#include "sgcl/Game/UI/UIToggleButton.h"
#include "sgcl/Game/UI/UIProgressBar.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

REGISTER_UI(ui_generic, UI_Generic)

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

	_LogDebug_("[UI_Generic] OnInit title=%p close=%p chkA=%p chkB=%p chkC=%p chkD=%p toggle=%p pbar1=%p pbar2=%p pbar3=%p pbar4=%p pbar5=%p pbar6=%p",
		pLabelTitle_, pBtnClose_, pChkA_, pChkB_, pChkC_, pChkD_, pToggleOpt_,
		pPbar1_, pPbar2_, pPbar3_, pPbar4_, pPbar5_, pPbar6_);
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
}
