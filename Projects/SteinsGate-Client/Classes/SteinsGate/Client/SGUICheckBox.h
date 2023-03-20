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
	static constexpr int IndexBackground = 0;
	static constexpr int IndexBackgroundDisabled = 1;
	static constexpr int IndexCross = 2;
	static constexpr int IndexCrossDisabled = 3;

	SGUICheckBox(SGUIGroup* parent, SGUICheckBoxInfo* checkBoxInfo);
	~SGUICheckBox() override;

	bool init() override;
	void load() override;
	void unload() override;


	void setCheck(bool checked);
	void setEnabled(bool enabled) override;
	bool isChecked() const;

	static SGUICheckBox* create(SGUIGroup* parent, SGUICheckBoxInfo* checkBoxInfo);

	bool onMouseUp(SGEventMouse* mouseEvent) override;

	int getCode() override;
	UIElementType_t getElementType() override { return UIElementType::CheckBox; }

	void setCallbackCheckStateChanged(const SGActionFn<SGUICheckBox*, bool>& fnCheckStateChangedCallback);
private:
	SGUICheckBoxInfo* m_pInfo;
	SGFrameTexture* m_pBackgroundTexture;
	SGFrameTexture* m_pBackgroundDisabledTexture;
	SGFrameTexture* m_pCrossTexture;
	SGFrameTexture* m_pCrossDisabledTexture;
	SGSprite* m_pBackgroundSprite;
	SGSprite* m_pBackgroundDisabledSprite;
	SGSprite* m_pCrossSprite;
	SGSprite* m_pCrossDisabledSprite;

	SGActionFn<SGUICheckBox*, bool> m_fnCheckStateChangedCallback;
	bool m_bChecked;
};