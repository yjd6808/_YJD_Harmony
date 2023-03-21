/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:17 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUIToggleButton : public SGUIElement
{
public:
	SGUIToggleButton(SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo);
	~SGUIToggleButton() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setVisibleState(State state);
	void setEnabled(bool enabled) override;
	int getToggleState() const { return m_iToggleState; }

	static SGUIToggleButton* create(SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo);
	void restoreState(State state) override;
	bool onMouseMove(SGEventMouse* mouseEvent) override;
	bool onMouseDown(SGEventMouse* mouseEvent) override;
	bool onMouseUp(SGEventMouse* mouseEvent) override;
	UIElementType_t getElementType() override { return UIElementType::ToggleButton; }
private:
	int m_iToggleState;
	SGUIToggleButtonInfo* m_pInfo;
	SGFrameTexture* m_pTexture[2][eMax];
	SGSprite* m_pSprite[2][eMax];
};