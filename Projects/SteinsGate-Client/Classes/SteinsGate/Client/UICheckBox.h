/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UICheckBox : public UIElement
{
public:
	static constexpr int TextureCount = 4;
	static constexpr int IndexBackground = 0;
	static constexpr int IndexBackgroundDisabled = 1;
	static constexpr int IndexCross = 2;
	static constexpr int IndexCrossDisabled = 3;

	static UICheckBox* create(UIMasterGroup* master, UIGroup* parent);
	static UICheckBox* create(UIMasterGroup* master, UIGroup* parent, UICheckBoxInfo* checkBoxInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::CheckBox; }

	UICheckBox(UIMasterGroup* master, UIGroup* parent);
	UICheckBox(UIMasterGroup* master, UIGroup* parent, UICheckBoxInfo* checkBoxInfo, bool infoOwner);
	~UICheckBox() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setCheck(bool checked);
	void setEnabled(bool enabled) override;
	void setUISize(const SGSize& size) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoCheckBox(UICheckBoxInfo* info, bool infoOwner);
	bool isChecked() const;

	UIElementType_t getElementType() override { return UIElementType::CheckBox; }
	SGString toString() override { return SGStringUtil::Format("체크박스(%d)", m_pInfo->Code); }
protected:
	bool onMouseUpContainedInternalDetail(SGEventMouse* mouseEvent) override;
private:
	UICheckBoxInfo* m_pInfo;
	FrameTexture* m_pTexture[TextureCount];
	SGSprite* m_pSprite[TextureCount];
	bool m_bChecked;
};