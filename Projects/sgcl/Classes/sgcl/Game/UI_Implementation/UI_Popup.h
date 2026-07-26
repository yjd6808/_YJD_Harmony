#pragma once

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/UIRegister.h"

#define SG_POPUP_NO_TIMEOUT	-1

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

	enum Attribute
	{
		eCloseWithEsc = 0b00000001,
		eTimeout = 0b00000010
	};

protected:
	void OnInit(const jc::CDataMap<>& _param) override;
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
	void SetTimeout(float _timeout);
	void SetClosed(bool _closed) { closed_ = _closed; }
	bool IsClosed() const { return closed_; }
	void Close();
	void Adjust();

private:
	UIGroup* pGroupHolder_;
	UISprite* pSpriteBackground_;
	UIGroup* pGroupButtonHolder_;
	UIButton* pBtnYes_;
	UIButton* pBtnNo_;
	UIButton* pBtnOk_;
	UILabel* pLabelText_;

	Type type_;
	jc::BitFlag<Attribute> attributeFlag_;
	jc::TimeCounterF openedTime_;
	float timeout_;
	bool closed_;

	PopupCallback yesCallback_;
	PopupCallback noCallback_;
	PopupCallback okCallback_;
	PopupCallback timeoutCallback_;
};

