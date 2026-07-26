#include "Game/UI_Implementation/UI_Generic.h"
#include "sgcl/Game/UI/UICheckBox.h"
#include "sgcl/Game/UI/UIEditBox.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UI_Generic::UI_Generic(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pLabelTitle_(nullptr)
, pLabelContent_(nullptr)
, pBtnClose_(nullptr)
, pChkOpt1_(nullptr)
, pChkOpt2_(nullptr)
, pEditInput_(nullptr)
{
}

void UI_Generic::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	pLabelTitle_ = FindElementByName<UILabel>("title");
	pLabelContent_ = FindElementByName<UILabel>("content");
	pBtnClose_ = FindElementByName<UIButton>("close");
	pChkOpt1_ = FindElementByName<UICheckBox>("chk_opt1");
	pChkOpt2_ = FindElementByName<UICheckBox>("chk_opt2");
	pEditInput_ = FindElementByName<UIEditBox>("edit_input");

	_LogDebug_("[UI_Generic] OnInit title=%p content=%p close=%p chk1=%p chk2=%p edit=%p",
		pLabelTitle_, pLabelContent_, pBtnClose_, pChkOpt1_, pChkOpt2_, pEditInput_);
}

void UI_Generic::OnLoaded()
{
}

void UI_Generic::OnUpdate(float _dt)
{
}

void UI_Generic::OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent)
{
	const char* name = _pElement->GetName();

	if (strcmp(name, "close") == 0)
	{
		removeFromParent();
		return;
	}

	if (strcmp(name, "btn_normal") == 0)      { _LogDebug_("[UI_Generic] btn_normal clicked"); }
	else if (strcmp(name, "btn_confirm") == 0) { _LogDebug_("[UI_Generic] btn_confirm clicked"); }
	else if (strcmp(name, "btn_cancel") == 0)  { _LogDebug_("[UI_Generic] btn_cancel clicked"); }
	else if (strcmp(name, "btn_apply") == 0)   { _LogDebug_("[UI_Generic] btn_apply clicked"); }
	else if (strcmp(name, "btn_delete") == 0)  { _LogDebug_("[UI_Generic] btn_delete clicked"); }
	else if (strcmp(name, "btn_more") == 0)    { _LogDebug_("[UI_Generic] btn_more clicked"); }
	else if (strcmp(name, "chk_opt1") == 0)    { _LogDebug_("[UI_Generic] chk_opt1 toggled: checked=%d", pChkOpt1_->IsChecked()); }
	else if (strcmp(name, "chk_opt2") == 0)    { _LogDebug_("[UI_Generic] chk_opt2 toggled: checked=%d", pChkOpt2_->IsChecked()); }
}

REGISTER_UI(ui_generic, UI_Generic)
