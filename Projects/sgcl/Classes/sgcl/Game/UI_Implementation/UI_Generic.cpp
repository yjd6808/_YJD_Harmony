#include "Game/UI_Implementation/UI_Generic.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UI_Generic::UI_Generic(UIGroupInfo* _pGroupInfo)
: UIRootGroup(_pGroupInfo)
, pLabelTitle_(nullptr)
, pLabelContent_(nullptr)
, pBtnClose_(nullptr)
{
}

void UI_Generic::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	pLabelTitle_ = FindElementByName<UILabel>("title");
	pLabelContent_ = FindElementByName<UILabel>("content");
	pBtnClose_ = FindElementByName<UIButton>("close");

	_LogDebug_("[UI_Generic] OnInit title=%p content=%p close=%p",
		pLabelTitle_, pLabelContent_, pBtnClose_);
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
}

REGISTER_UI(ui_generic, UI_Generic)
