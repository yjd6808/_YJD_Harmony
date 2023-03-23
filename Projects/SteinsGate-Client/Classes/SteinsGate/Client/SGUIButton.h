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
	SGUIButton(SGUIMasterGroup* master, SGUIGroup* parent, SGUIButtonInfo* btnInfo);
	~SGUIButton() override;
	 
	bool init() override;
	void load() override;
	void unload() override;
	
	void setVisibleState(State state);
	void setEnabled(bool enabled) override;

	static SGUIButton* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIButtonInfo* btnInfo);
	void restoreState(State state) override;

	void onMouseEnterDetail(SGEventMouse* mouseEvent) override;
	void onMouseLeaveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseMoveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownDetail(SGEventMouse* mouseEvent) override;
	void onMouseUpDetail(SGEventMouse* mouseEvent) override;

	UIElementType_t getElementType() override { return UIElementType::Button; }
	SGString toString() override { return SGStringUtil::Format("버튼(%d)", m_pInfo->Code); }
private:
	SGUIButtonInfo* m_pInfo;
	SGFrameTexture* m_pTexture[eMax];
	SGSprite* m_pSprite[eMax];
};