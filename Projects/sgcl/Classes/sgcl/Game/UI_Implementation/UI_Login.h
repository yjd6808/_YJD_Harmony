/*
 * 작성자: 윤정도
 * =====================
 * 로그인 화면 (sgui 기반 재구현)
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"

class UI_Login : public sgui::Window
{
public:
	enum class Tab
	{
		eHangame,
		eDnf
	};

	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;
	virtual bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode) override;

	void SetTab(Tab _tab);
	void Login();

private:
	sgui::TextBlock* pLabelSource_ = nullptr;
	sgui::TextBlock* pLabelDeveloper_ = nullptr;

	sgui::Border* pLoginBox_ = nullptr;
	sgui::ToggleButton* pToggleHangameLogin_ = nullptr;
	sgui::ToggleButton* pToggleDnfLogin_ = nullptr;
	sgui::Button* pBtnStart_ = nullptr;
	sgui::Button* pBtnTerminate_ = nullptr;
	sgui::TextBox* pEditId_ = nullptr;
	sgui::TextBox* pEditPassword_ = nullptr;

	Tab tab_ = Tab::eDnf;
};
