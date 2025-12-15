/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:09:52 AM
 * =====================
 * 팝업은 마스터그룹이지만 그룹마스터가 관리하지 않고 팝업매니저가 관리토록한다.
 */


#pragma once

#include <SteinsGate/Client/UIRootGroup.h>

#define SG_POPUP_NO_TIMEOUT	-1	// 타임아웃 시간 음수로 지정시, 타임아웃 기능 사용안하도록 함

using PopupCallback = SGActionFn<>;

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

	void onUpdate(float _dt) override;
	void OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseScrollInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

public:
	void setType(Type _type);
	Type getType() const { return type_; }

	void setTextHAlign(HAlignment_t _hAlign) { labelText_->setHAlignment(_hAlign); }
	void setTextVAlign(VAlignment_t _vAlign) { labelText_->setVAlignment(_vAlign); }
	int getAttribute() const { return attributeFlag_.Value; }

	void setText(const std::string& _text);
	void setYesCallback(const PopupCallback& _fnYes);
	void setNoCallback(const PopupCallback& _fnNo);
	void setOkCallback(const PopupCallback& _fnOk);
	void setCloseWithEsc(bool _closeWithEsc);
	void setTimeoutCallback(const PopupCallback& _fnTimeout);
	void setTimeout(float _timeout); // 음수 시간넣으면 타임아웃 미적용
	void setClosed(bool _closed) { closed_ = _closed; }
	bool isClosed() const { return closed_; }
	void close();
	void adjust();

private:
	// #define UI_POPUP_GROUP_HOLDER
	// #define UI_POPUP_HOLDER_SPRITE_BACKGROUND
	// #define UI_POPUP_HOLDER_LABEL_TEXT
	// #define UI_POPUP_HOLDER_GROUP_BTN_HOLDER
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_YES
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_NO
	// #define UI_POPUP_HOLDER_BTN_HOLDER_BUTTON_OK

	UIGroup* groupHolder_;
	UISprite* spriteBackground_;
	UIGroup* groupButtonHolder_;
	UIButton* yesButton_;
	UIButton* noButton_;
	UIButton* okButton_;
	UILabel* labelText_;

	// =========================================

	Type type_;
	SGBitFlag<Attribute> attributeFlag_;
	SGTimeCounterF openedTime_; // 열려있던 시간
	float timeout_;
	bool closed_;

	PopupCallback yesCallback_;
	PopupCallback noCallback_;
	PopupCallback okCallback_;
	PopupCallback timeoutCallback_;
};
