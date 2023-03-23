/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUICheckBox : public SGUIElement
{
public:
	static constexpr int TextureCount = 4;
	static constexpr int IndexBackground = 0;
	static constexpr int IndexBackgroundDisabled = 1;
	static constexpr int IndexCross = 2;
	static constexpr int IndexCrossDisabled = 3;

	SGUICheckBox(SGUIMasterGroup* master, SGUIGroup* parent, SGUICheckBoxInfo* checkBoxInfo);
	~SGUICheckBox() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setCheck(bool checked);
	void setEnabled(bool enabled) override;
	bool isChecked() const;

	static SGUICheckBox* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUICheckBoxInfo* checkBoxInfo);

	bool onMouseUpContainedDetail(SGEventMouse* mouseEvent) override;

	UIElementType_t getElementType() override { return UIElementType::CheckBox; }
	SGString toString() override { return SGStringUtil::Format("체크박스(%d)", m_pInfo->Code); }
private:
	SGUICheckBoxInfo* m_pInfo;
	SGFrameTexture* m_pTexture[TextureCount];
	SGSprite* m_pSprite[TextureCount];
	bool m_bChecked;
};