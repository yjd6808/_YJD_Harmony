/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 5:18:04 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUIButton : public SGUIElement
{
public:
	SGUIButton(SGUIGroup* parent, SGUIButtonInfo* btnInfo);
	~SGUIButton() override;
	 
	bool init() override;
	void load() override;
	void unload() override;
	
	void setVisibleState(State state);
	void setEnabled(bool enabled) override;

	static SGUIButton* create(SGUIGroup* parent, SGUIButtonInfo* btnInfo);
	void restoreState(State state) override;
	bool onMouseMove(SGEventMouse* mouseEvent) override;
	bool onMouseDown(SGEventMouse* mouseEvent) override;
	bool onMouseUp(SGEventMouse* mouseEvent) override;
	bool onMouseScroll(SGEventMouse* mouseEvent) override;
	UIElementType_t getElementType() override { return UIElementType::Button; }
private:
	SGUIButtonInfo* m_pInfo;
	SGFrameTexture* m_pTexture[eMax];
	SGSprite* m_pSprite[eMax];
};