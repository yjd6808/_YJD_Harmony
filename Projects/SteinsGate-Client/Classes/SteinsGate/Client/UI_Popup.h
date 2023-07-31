/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:09:52 AM
 * =====================
 * 팝업은 마스터그룹이지만 그룹마스터가 관리하지 않고 팝업매니저가 관리토록한다.
 */


#pragma once

#include <SteinsGate/Client/UIMasterGroup.h>



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
		eMessaging			= 0b00000001,
		eCanCloseWithEsc	= 0b00000010,
		eTimed				= 0b00000100,
		eElse2				= 0b00001000
	};



protected:
	void onInit() override;
	void onLoaded() override;
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

	void setAttribute(Attribute attr) { m_iAttribute = attr; }
	void setTextHAlign(HAlignment_t halign) { m_pLabelText->setHAlignment(halign); }
	void setTextVAlign(VAlignment_t valign) { m_pLabelText->setVAlignment(valign); }
	void addAttribute(Attribute attr) { m_iAttribute |= attr; }
	void removeAttribute(Attribute attr) { m_iAttribute &= ~attr; }
	bool hasAttribute(Attribute attr) { return (m_iAttribute & attr) == attr; }
	int getAttribute() const { return m_iAttribute; }

	void setText(const std::string& text);
	void setYesCallback(const SGActionFn<>& fnYes);
	void setNoCallback(const SGActionFn<>& fnNo);
	void setOkCallback(const SGActionFn<>& fnOk);
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
	int m_iAttribute;
	bool m_bClosed;

	SGActionFn<> m_fnYes;
	SGActionFn<> m_fnNo;
	SGActionFn<> m_fnOk;
};
