/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:09:52 AM
 * =====================
 * 팝업은 마스터그룹이지만 그룹마스터가 관리하지 않고 팝업매니저가 관리토록한다.
 */


#pragma once

#include "sgcl/Game/UI/UIRootGroup.h"

#define SG_POPUP_NO_TIMEOUT	-1	// 타임아웃 시간 음수로 지정시, 타임아웃 기능 사용안하도록 함

using PopupCallback = jc::Action<>;

class UI_Popup : public UIRootGroup
{
public:
	UI_Popup(UIGroupInfo* _pGroupInfo);

	enum class Type
	{
		eNone,
		eYesNo,
		eOk
	};

	// 팝업이 여러 속성을 가질 수도 있지 않을까 해서 일단 둠
	enum Attribute
	{
		eCloseWithEsc = 0b00000001,
		eTimeout = 0b00000010
	};

protected:
	void OnInit() override;
	void OnLoaded() override;

	void OnAdded() override;
	void OnRemoved() override;

	void OnUpdate(float _dt) override;
	void OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseScrollInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

public:
	void SetType(Type _type);
	Type GetType() const { return type_; }

	void SetTextHAlign(HAlignment_t _hAlign) { pLabelText_->setHAlignment(_hAlign); }
	void SetTextVAlign(VAlignment_t _vAlign) { pLabelText_->setVAlignment(_vAlign); }
	int GetAttribute() const { return attributeFlag_.Value; }

	void SetText(const std::string& _text);
	void SetYesCallback(const PopupCallback& _fnYes);
	void SetNoCallback(const PopupCallback& _fnNo);
	void SetOkCallback(const PopupCallback& _fnOk);
	void SetCloseWithEsc(bool _closeWithEsc);
	void SetTimeoutCallback(const PopupCallback& _fnTimeout);
	void SetTimeout(float _timeout); // 음수 시간넣으면 타임아웃 미적용
	void SetClosed(bool _closed) { closed_ = _closed; }
	bool IsClosed() const { return closed_; }
	void Close();
	void Adjust();

private:
	// #define UI_POPUP_GROUP_HOLDER
	// #define UI_POPUP_HOLDER_SPRITE_BACKGROUND
	// #define UI_POPUP_HOLDER_LABEL_TEXT
	// #define UI_POPUP_HOLDER_GROUP_BTN_HOLDER
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK

	UIGroup* pGroupHolder_;
	UISprite* pSpriteBackground_;
	UIGroup* pGroupButtonHolder_;
	UIButton* pBtnYes_;
	UIButton* pBtnNo_;
	UIButton* pBtnOk_;
	UILabel* pLabelText_;

	// =========================================

	Type type_;
	jc::BitFlag<Attribute> attributeFlag_;
	jc::TimeCounterF openedTime_; // 열려있던 시간
	float timeout_;
	bool closed_;

	PopupCallback yesCallback_;
	PopupCallback noCallback_;
	PopupCallback okCallback_;
	PopupCallback timeoutCallback_;
};
