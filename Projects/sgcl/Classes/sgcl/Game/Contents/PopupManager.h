/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:26:05 AM
 * =====================
 *
 */

#pragma once

#include "sgcl/Game/UI_Implementation/UI_Popup.h"

class PopupManager : public jc::SingletonPointer<PopupManager>
{
	friend class TSingleton;
	PopupManager();
	~PopupManager();

public:
	void SetWidth(float _width);
	void SetPadding(float _padding);

	UI_Popup* ShowYesNo(
		const std::string& _text,
		const PopupCallback& _yes = nullptr,
		const PopupCallback& _no = nullptr,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top);

	UI_Popup* ShowYesNo(
		const std::string& _text,
		int _tag,
		const PopupCallback& _yes = nullptr,
		const PopupCallback& _no = nullptr,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top);

	UI_Popup* ShowOk(
		const std::string& _text,
		const PopupCallback& _ok = nullptr,
		bool _closeWithEsc = true,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top);

	UI_Popup* ShowOk(
		const std::string& _text,
		int _tag,
		const PopupCallback& _ok = nullptr,
		bool _closeWithEsc = true,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top);

	UI_Popup* ShowNone(
		const std::string& _text,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Center,
		VAlignment_t _valign = VAlignment::Center);

	UI_Popup* ShowNone(
		const std::string& _text,
		int _tag,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Center,
		VAlignment_t _valign = VAlignment::Center);

	bool Close(UI_Popup* _pPopup);
	bool CloseByTag(int _tag);
	UI_Popup* FindByTag(int _tag);
	int CloseAll();
	void ReleaseAll();

	float GetWidth() const { return width_; }
	float GetPadding() const { return padding_; }

protected:
	UI_Popup* Pop();
	UI_Popup* CreatePopup();

private:
	float width_;
	float padding_;

	UIGroupInfo* pPopupInfo_;

	jc::Vector<UI_Popup*> opendList_;
	jc::ArrayQueue<UI_Popup*> popupPool_;
};
