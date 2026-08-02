/*
 * 작성자: 윤정도
 * =====================
 * 팝업 화면 (sgui 기반 재구현)
 * PopupManager가 사용하는 API는 기존과 동일하게 유지한다.
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"

#define SG_POPUP_NO_TIMEOUT	-1

using PopupCallback = jc::Action<>;

class UI_Popup : public sgui::Window
{
public:
	enum class Type
	{
		eNone,
		eYesNo,
		eOk
	};

	static UI_Popup* Create();

	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnUpdate(float _dt) override;
	virtual bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode) override;

	// ==================== PopupManager용 API (기존 유지) ====================
	void SetType(Type _type);
	Type GetType() const { return type_; }

	void SetTextHAlign(HAlignment_t _hAlign);
	void SetTextVAlign(VAlignment_t _vAlign);

	void SetText(const std::string& _text);
	void SetYesCallback(const PopupCallback& _fnYes) { yesCallback_ = _fnYes; }
	void SetNoCallback(const PopupCallback& _fnNo) { noCallback_ = _fnNo; }
	void SetOkCallback(const PopupCallback& _fnOk) { okCallback_ = _fnOk; }
	void SetCloseWithEsc(bool _closeWithEsc) { SetCloseWithEscape(_closeWithEsc); }
	void SetTimeoutCallback(const PopupCallback& _fnTimeout) { timeoutCallback_ = _fnTimeout; }
	void SetTimeout(float _timeout);
	void SetClosed(bool _closed) { closed_ = _closed; }
	bool IsClosed() const { return closed_; }
	void ClosePopup();	// PopupManager를 통해 닫는다. (풀 반환)
	void Adjust();

private:
	sgui::Border* pHolder_ = nullptr;
	sgui::TextBlock* pTextBlock_ = nullptr;
	sgui::StackPanel* pButtonPanel_ = nullptr;
	sgui::Button* pBtnYes_ = nullptr;
	sgui::Button* pBtnNo_ = nullptr;
	sgui::Button* pBtnOk_ = nullptr;

	Type type_ = Type::eNone;
	float timeout_ = SG_POPUP_NO_TIMEOUT;
	float elapsed_ = 0.0f;
	bool closed_ = false;

	PopupCallback yesCallback_;
	PopupCallback noCallback_;
	PopupCallback okCallback_;
	PopupCallback timeoutCallback_;
};
