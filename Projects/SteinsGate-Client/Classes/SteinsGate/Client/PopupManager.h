/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:26:05 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UI_Popup.h>

class PopupManager : public JCore::SingletonPointer<PopupManager>
{
	friend class TSingleton;
	PopupManager();
	~PopupManager() override;
public:
	void setWidth(float width);
	void setPadding(float padding);

	UI_Popup* showYesNo(
		const std::string& text, 
		const PopupCallback& yes = nullptr, 
		const PopupCallback& no = nullptr,
		bool closeWithEsc = false,
		float timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& timeoutFn = nullptr,
		HAlignment_t halign = HAlignment::Left,
		VAlignment_t valign = VAlignment::Top
	);

	UI_Popup* showYesNo(
		const std::string& text,
		int tag,
		const PopupCallback& yes = nullptr,
		const PopupCallback& no = nullptr,
		bool closeWithEsc = false,
		float timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& timeoutFn = nullptr,
		HAlignment_t halign = HAlignment::Left,
		VAlignment_t valign = VAlignment::Top
	);


	UI_Popup* showOk(
		const std::string& text,
		const PopupCallback& ok = nullptr,
		bool closeWithEsc = true,
		float timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& timeoutFn = nullptr,
		HAlignment_t halign = HAlignment::Left,
		VAlignment_t valign = VAlignment::Top
	);

	UI_Popup* showOk(
		const std::string& text,
		int tag,
		const PopupCallback& ok = nullptr,
		bool closeWithEsc = true,
		float timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& timeoutFn = nullptr,
		HAlignment_t halign = HAlignment::Left,
		VAlignment_t valign = VAlignment::Top
	);

	UI_Popup* showNone(
		const std::string& text,
		bool closeWithEsc = false,
		float timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& timeoutFn = nullptr,
		HAlignment_t halign = HAlignment::Center,
		VAlignment_t valign = VAlignment::Center
	);

	UI_Popup* showNone(
		const std::string& text,
		int tag,
		bool closeWithEsc = false,
		float timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& timeoutFn = nullptr,
		HAlignment_t halign = HAlignment::Center, 
		VAlignment_t valign = VAlignment::Center
	);

	bool close(UI_Popup* popup);
	bool closeByTag(int tag);
	UI_Popup* findByTag(int tag);
	int closeAll();
	void releaseAll();

	float getWidth() const { return m_fWidth; }
	float getPadding() const { return m_fPadding; }
protected:
	UI_Popup* pop();
	UI_Popup* createPopup();
private:
	float m_fWidth;
	float m_fPadding;

	SGVector<UI_Popup*> m_vOpendList;
	SGArrayQueue<UI_Popup*> m_qPopupPool;
};
