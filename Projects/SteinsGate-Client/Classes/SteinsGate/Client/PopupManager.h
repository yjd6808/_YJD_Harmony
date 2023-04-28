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
	void setPopupSize(float width, float height);
	void setPopupSize(const SGSize& size) { setPopupSize(size.width, size.height); }
	void showYesNo(const std::string& text, const SGActionFn<>& yes = nullptr, const SGActionFn<>& no = nullptr);
	void showOk(const std::string& text, const SGActionFn<>& ok = nullptr);
private:
	SGSize m_PopupSize;
	SGVector<UI_Popup*> m_vPopupList;
};
