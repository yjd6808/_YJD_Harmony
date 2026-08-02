/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "Game/UI_Implementation/UI_Login.h"

#include "sgcl/Define/Define_Popup.h"
#include "sgcl/Net/Send/S_AUTH.h"
#include "sgcl/Scene/Scene_World.h"
#include "sgcl/Net/NetCore.h"
#include "sgcl/Game/Contents/Contents.h"

#include "sgcl/Net/Component/AuthenticationComponent.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

using namespace sgui;

void UI_Login::OnInit(const CDataMap<>& _param)
{
	UNUSED(_param);

	// ==================== XML 우선 (layout/ui_login.xml 병합 시) ====================
	// sg-main 방식: XML로 로드된 Element를 OnInit 단에서 FindElementByName으로 멤버에 할당한다.
	if (IsXmlLoaded())
	{
		pLabelSource_ = FindElementByName<TextBlock>("source");
		pLabelDeveloper_ = FindElementByName<TextBlock>("developer");

		pLoginBox_ = FindElementByName<Border>("login_box");
		pToggleHangameLogin_ = FindElementByName<ToggleButton>("hangame_login");
		pToggleDnfLogin_ = FindElementByName<ToggleButton>("dnf_login");
		pBtnStart_ = FindElementByName<sgui::Button>("start");
		pBtnTerminate_ = FindElementByName<sgui::Button>("terminate");
		pEditId_ = FindElementByName<TextBox>("id");
		pEditPassword_ = FindElementByName<TextBox>("pw");

		// 코드에서만 가능한 속성/이벤트를 OnInit에서 연결한다. (sg-main 동일)
		if (pEditId_)
		{
			pEditId_->SetMaxLength(Const::StringLen::AccountId);
		}

		if (pEditPassword_)
		{
			pEditPassword_->SetPasswordMode(true);
			pEditPassword_->SetMaxLength(Const::StringLen::AccountPass);
			pEditPassword_->AddHandler(ueTextSubmit, [this](UIElement*, UIEvent*) { Login(); });
		}

		if (pToggleHangameLogin_)
		{
			pToggleHangameLogin_->SetClickCallback([this](ButtonBase*) { SetTab(Tab::eHangame); });
		}

		if (pToggleDnfLogin_)
		{
			pToggleDnfLogin_->SetClickCallback([this](ButtonBase*) { SetTab(Tab::eDnf); });
		}

		if (pBtnStart_)
		{
			pBtnStart_->SetClickCallback([this](ButtonBase*) { Login(); });
		}

		if (pBtnTerminate_)
		{
			pBtnTerminate_->SetClickCallback([](ButtonBase*) { g_cWorld.Terminate(); });
		}

		_LogDebug_("[UI_Login] OnInit(XML) source=%p developer=%p login_box=%p hangame=%p dnf=%p start=%p terminate=%p id=%p pw=%p",
			pLabelSource_, pLabelDeveloper_, pLoginBox_,
			pToggleHangameLogin_, pToggleDnfLogin_, pBtnStart_, pBtnTerminate_, pEditId_, pEditPassword_);
		return;
	}

	// ==================== 코드 빌드 폴백 (XML 없음) ====================
	// 배경
	Border* pBackground = Border::Create();
	pBackground->setName("background");
	pBackground->SetBackground(SolidColorBrush::Create(0.0f, 0.0f, 0.0f, 1.0f));
	pBackground->SetHorizontalAlignment(HorizontalAlignment::Stretch);
	pBackground->SetVerticalAlignment(VerticalAlignment::Stretch);
	AddChild(pBackground);

	// ==================== 로그인 박스 ====================
	StackPanel* pLoginStack = StackPanel::Create(Orientation::Vertical);
	pLoginStack->SetSpacing(8.0f);

	// 로그인 탭 (한게임 / 던파)
	StackPanel* pTabStack = StackPanel::Create(Orientation::Horizontal);
	pTabStack->SetSpacing(4.0f);
	pTabStack->SetHorizontalAlignment(HorizontalAlignment::Center);

	pToggleHangameLogin_ = ToggleButton::Create("한게임 로그인");
	pToggleHangameLogin_->setName("hangame_login");
	pToggleHangameLogin_->SetClickCallback([this](ButtonBase*) { SetTab(Tab::eHangame); });
	pTabStack->AddChild(pToggleHangameLogin_);

	pToggleDnfLogin_ = ToggleButton::Create("던파 로그인");
	pToggleDnfLogin_->setName("dnf_login");
	pToggleDnfLogin_->SetClickCallback([this](ButtonBase*) { SetTab(Tab::eDnf); });
	pTabStack->AddChild(pToggleDnfLogin_);

	pLoginStack->AddChild(pTabStack);

	// 아이디 / 비밀번호
	pEditId_ = TextBox::Create();
	pEditId_->setName("id");
	pEditId_->SetPlaceholder("아이디");
	pEditId_->SetMaxLength(Const::StringLen::AccountId);
	pEditId_->SetWidth(240.0f);
	pLoginStack->AddChild(pEditId_);

	pEditPassword_ = TextBox::Create();
	pEditPassword_->setName("pw");
	pEditPassword_->SetPlaceholder("비밀번호");
	pEditPassword_->SetPasswordMode(true);
	pEditPassword_->SetMaxLength(Const::StringLen::AccountPass);
	pEditPassword_->SetWidth(240.0f);
	pEditPassword_->AddHandler(ueTextSubmit, [this](UIElement*, UIEvent*) { Login(); });
	pLoginStack->AddChild(pEditPassword_);

	// 시작 / 종료 버튼
	StackPanel* pButtonStack = StackPanel::Create(Orientation::Horizontal);
	pButtonStack->SetSpacing(8.0f);
	pButtonStack->SetHorizontalAlignment(HorizontalAlignment::Center);

	pBtnStart_ = sgui::Button::Create("게임 시작");
	pBtnStart_->setName("start");
	pBtnStart_->SetClickCallback([this](ButtonBase*) { Login(); });
	pButtonStack->AddChild(pBtnStart_);

	pBtnTerminate_ = sgui::Button::Create("종료");
	pBtnTerminate_->setName("terminate");
	pBtnTerminate_->SetClickCallback([](ButtonBase*) { g_cWorld.Terminate(); });
	pButtonStack->AddChild(pBtnTerminate_);

	pLoginStack->AddChild(pButtonStack);

	pLoginBox_ = Border::Create();
	pLoginBox_->setName("login_box");
	pLoginBox_->SetBackground(SolidColorBrush::Create(0.0f, 0.0f, 0.0f, 0.6f));
	pLoginBox_->SetPadding(Thickness(24.0f));
	pLoginBox_->SetHorizontalAlignment(HorizontalAlignment::Center);
	pLoginBox_->SetVerticalAlignment(VerticalAlignment::Center);
	pLoginBox_->SetChild(pLoginStack);
	AddChild(pLoginBox_);

	// ==================== 좌하단 라벨 ====================
	StackPanel* pInfoStack = StackPanel::Create(Orientation::Vertical);
	pInfoStack->SetSpacing(2.0f);
	pInfoStack->SetHorizontalAlignment(HorizontalAlignment::Left);
	pInfoStack->SetVerticalAlignment(VerticalAlignment::Bottom);
	pInfoStack->SetMargin(Thickness(12.0f));

	pLabelSource_ = TextBlock::Create("SteinsGate Client");
	pLabelSource_->setName("source");
	pInfoStack->AddChild(pLabelSource_);

	pLabelDeveloper_ = TextBlock::Create("작성자: 윤정도");
	pLabelDeveloper_->setName("developer");
	pInfoStack->AddChild(pLabelDeveloper_);

	AddChild(pInfoStack);
}

void UI_Login::OnLoaded()
{
	SetTab(tab_);
}

bool UI_Login::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		Login();
		return true;
	}

	return false;
}

void UI_Login::SetTab(Tab _tab)
{
	tab_ = _tab;
	if (pToggleHangameLogin_)
	{
		pToggleHangameLogin_->SetIsChecked(tab_ == Tab::eHangame);
	}
	if (pToggleDnfLogin_)
	{
		pToggleDnfLogin_->SetIsChecked(tab_ == Tab::eDnf);
	}
}

void UI_Login::Login()
{
	const std::string id = pEditId_->GetText();
	const std::string password = pEditPassword_->GetText();

	if (id.length() == 0 || password.length() == 0)
	{
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_REQUIRED_ID_PASS"));
		return;
	}

	g_cNet.authentication_.SetAccountIdPass(id.c_str(), password.c_str());

	if (!g_cNet.ConnectAuthTcp())
	{
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("CONNECT_AUTH_FAILED_UNCONNECTABLE_STATE"));
		return;
	}

	std::string msg = StringUtils::format(SG_LOCAL_RAW("CONNECT_SERVER"), ServerType::Name[ServerType::Auth]);
	sg::Contents.PopupManager->ShowNone(msg, DEF_POPUP_CONNECT_AUTH);
}

REGISTER_WINDOW(UI_Login, "ui_login")
