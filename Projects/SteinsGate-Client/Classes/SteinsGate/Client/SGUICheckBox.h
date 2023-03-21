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
	UIElementType_t getElementType() override { return UIElementType::CheckBox; }

	void setCallbackCheckStateChanged(const SGActionFn<SGUICheckBox*, bool>& fnCheckStateChangedCallback);
private:
	SGUICheckBoxInfo* m_pInfo;
	SGFrameTexture* m_pTexture[TextureCount];
	SGSprite* m_pSprite[TextureCount];
	SGActionFn<SGUICheckBox*, bool> m_fnCheckStateChangedCallback;
	bool m_bChecked;
};