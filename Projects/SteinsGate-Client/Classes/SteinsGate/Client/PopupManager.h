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
	~PopupManager();

public:
	void setWidth(float _width);
	void setPadding(float _padding);

	UI_Popup* showYesNo(
		const std::string& _text,
		const PopupCallback& _yes = nullptr,
		const PopupCallback& _no = nullptr,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top
	);

	UI_Popup* showYesNo(
		const std::string& _text,
		int _tag,
		const PopupCallback& _yes = nullptr,
		const PopupCallback& _no = nullptr,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top
	);

	UI_Popup* showOk(
		const std::string& _text,
		const PopupCallback& _ok = nullptr,
		bool _closeWithEsc = true,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top
	);

	UI_Popup* showOk(
		const std::string& _text,
		int _tag,
		const PopupCallback& _ok = nullptr,
		bool _closeWithEsc = true,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Left,
		VAlignment_t _valign = VAlignment::Top
	);

	UI_Popup* showNone(
		const std::string& _text,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Center,
		VAlignment_t _valign = VAlignment::Center
	);

	UI_Popup* showNone(
		const std::string& _text,
		int _tag,
		bool _closeWithEsc = false,
		float _timeout = SG_POPUP_NO_TIMEOUT,
		const PopupCallback& _timeoutFn = nullptr,
		HAlignment_t _halign = HAlignment::Center,
		VAlignment_t _valign = VAlignment::Center
	);

	bool close(UI_Popup* _pPopup);
	bool closeByTag(int _tag);
	UI_Popup* findByTag(int _tag);
	int closeAll();
	void releaseAll();

	float getWidth() const { return width_; }
	float getPadding() const { return padding_; }

protected:
	UI_Popup* pop();
	UI_Popup* createPopup();

private:
	float width_;
	float padding_;

	SGVector<UI_Popup*> opendList_;
	SGArrayQueue<UI_Popup*> popupPool_;
};
