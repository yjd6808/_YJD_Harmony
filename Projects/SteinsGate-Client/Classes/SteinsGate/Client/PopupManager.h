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
	UI_Popup* showYesNo(const std::string& text, const SGActionFn<>& yes = nullptr, const SGActionFn<>& no = nullptr);
	UI_Popup* showYesNo(const std::string& text, int tag, const SGActionFn<>& yes = nullptr, const SGActionFn<>& no = nullptr);
	UI_Popup* showOk(const std::string& text, const SGActionFn<>& ok = nullptr);
	UI_Popup* showOk(const std::string& text, int tag, const SGActionFn<>& ok = nullptr);
	UI_Popup* showNone(const std::string& text, int tag, HAlignment_t halign = HAlignment::Center, VAlignment_t valign = VAlignment::Center);
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
