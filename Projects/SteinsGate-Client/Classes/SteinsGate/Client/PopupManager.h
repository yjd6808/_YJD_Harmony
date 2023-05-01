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
	void showYesNo(const std::string& text, const SGActionFn<>& yes = nullptr, const SGActionFn<>& no = nullptr);
	void showOk(const std::string& text, const SGActionFn<>& ok = nullptr);
	void close(UI_Popup* popup);
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
