/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:09:52 AM
 * =====================
 * 팝업은 마스터그룹이지만 그룹마스터가 관리하지 않고 팝업매니저가 관리토록한다.
 */


#pragma once

#include <SteinsGate/Client/UIMasterGroup.h>

#define SG_POPUP_NO_TIMEOUT	-1	// 타임아웃 시간 음수로 지정시, 타임아웃 기능 사용안하도록 함

using PopupCallback = SGActionFn<>;
class UI_Popup : public UIMasterGroup
{
public:
	UI_Popup(UIGroupInfo* groupInfo);

	enum class Type
	{
		eNone,
		eYesNo,
		eOk
	};


	// 팝업이 여러 속성을 가질 수도 있지 않을까 해서 일단 둠
	enum Attribute
	{
		eCloseWithEsc		= 0b00000001,
		eTimeout			= 0b00000010
	};

protected:
	void onInit() override;
	void onLoaded() override;

	void onAdded() override;
	void onRemoved() override;

	void onUpdate(float dt) override;
	void onMouseUp(UIElement* element, SGEventMouse* mouseEvent) override;
	bool onMouseMoveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownDetail(SGEventMouse* mouseEvent) override;
	bool onMouseScrollDetail(SGEventMouse* mouseEvent) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
public:
	void setType(Type type);
	Type getType() const { return m_eType; }

	void setTextHAlign(HAlignment_t halign) { m_pLabelText->setHAlignment(halign); }
	void setTextVAlign(VAlignment_t valign) { m_pLabelText->setVAlignment(valign); }
	int getAttribute() const { return m_AttributeFlag.Value; }

	void setText(const std::string& text);
	void setYesCallback(const PopupCallback& fnYes);
	void setNoCallback(const PopupCallback& fnNo);
	void setOkCallback(const PopupCallback& fnOk);
	void setCloseWithEsc(bool closeWithEsc);
	void setTimeoutCallback(const PopupCallback& fnTimeout);
	void setTimeout(float timeout);	// 음수 시간넣으면 타임아웃 미적용
	void setClosed(bool closed) { m_bClosed = closed; }
	bool isClosed() const { return m_bClosed; }
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

	UIGroup* m_pGroupHolder;
	UISprite* m_pSpriteBackground;
	UIGroup* m_pGroupButtonHolder;
	UIButton* m_pBtnYes;
	UIButton* m_pBtnNo;
	UIButton* m_pBtnOk;
	UILabel* m_pLabelText;

	// =========================================

	Type m_eType;
	SGBitFlag<Attribute> m_AttributeFlag;
	SGTimeCounterF m_OpenedTime;			// 열려있던 시간
	float m_fTimeout;
	bool m_bClosed;

	SGActionFn<> m_fnYes;
	SGActionFn<> m_fnNo;
	SGActionFn<> m_fnOk;
	SGActionFn<> m_fnTimeout;
};
