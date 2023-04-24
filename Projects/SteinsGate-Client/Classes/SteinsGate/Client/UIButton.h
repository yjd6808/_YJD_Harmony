/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 5:18:04 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UIButton : public UIElement
{
public:
	UIButton(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo);
	~UIButton() override;
	 
	bool init() override;
	void load() override;
	void unload() override;
	
	void setVisibleState(State state);
	void setEnabled(bool enabled) override;

	static UIButton* create(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo);
	void restoreState(State state) override;

	void onMouseEnterDetail(SGEventMouse* mouseEvent) override;
	void onMouseLeaveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseMoveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownDetail(SGEventMouse* mouseEvent) override;
	void onMouseUpDetail(SGEventMouse* mouseEvent) override;

	UIElementType_t getElementType() override { return UIElementType::Button; }
	SGString toString() override { return SGStringUtil::Format("버튼(%d)", m_pInfo->Code); }
private:
	UIButtonInfo* m_pInfo;
	FrameTexture* m_pTexture[eMax];
	SGSprite* m_pSprite[eMax];
};