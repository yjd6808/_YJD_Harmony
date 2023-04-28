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
		eYesNo,
		eOk
	};
protected:
	void onInit() override;
	void onLoaded() override;
public:
	void setType(Type type);
	Type getType() const { return m_eType; }
	void setPopupSize(const SGSize& size);
	void setText(const std::string& text);
	void setYesCallback(const SGActionFn<>& fnYes) { m_fnYes = fnYes; }
	void setNoCallback(const SGActionFn<>& fnNo) { m_fnNo = fnNo; }
	void setOkCallback(const SGActionFn<>& fnOk) { m_fnOk = fnOk; }
	void adjust();
	void setPadding(float padding) { m_fPadding = padding; }

	// 홀더 박스 내부 기준으로의 에리어임
	SGRect getTextArea();
	SGRect getButtonArea();
private:

	// #define POPUP_GROUP_HOLDER
	// #define POPUP_HOLDER_SPRITE_BACKGROUND
	// #define POPUP_HOLDER_BUTTON_YES
	// #define POPUP_HOLDER_BUTTON_NO
	// #define POPUP_HOLDER_BUTTON_OK
	// #define POPUP_HOLDER_LABEL_TEXT

	UIGroup* m_pGroupHolder;
	UISprite* m_pSpriteBackground;
	UIButton* m_pBtnYes;
	UIButton* m_pBtnNo;
	UIButton* m_pBtnOk;
	UILabel* m_pLabelText;

	// =========================================

	Type m_eType;
	float m_fPadding;

	SGActionFn<> m_fnYes;
	SGActionFn<> m_fnNo;
	SGActionFn<> m_fnOk;
};
