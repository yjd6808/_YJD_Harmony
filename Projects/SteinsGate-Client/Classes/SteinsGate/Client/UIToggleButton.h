/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:17 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIElement.h>

class UIToggleButton : public UIElement
{
	static constexpr int StateOne = 0;
	static constexpr int StateTwo = 1;
public:
	UIToggleButton(UIMasterGroup* master, UIGroup* parent, UIToggleButtonInfo* btnInfo);
	~UIToggleButton() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setVisibleState(State state);
	void setEnabled(bool enabled) override;
	int getToggleState() const { return m_iToggleState; }

	static UIToggleButton* create(UIMasterGroup* master, UIGroup* parent, UIToggleButtonInfo* btnInfo);
	void restoreState(State state) override;

	void onMouseEnterDetail(SGEventMouse* mouseEvent) override;
	void onMouseLeaveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseMoveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownDetail(SGEventMouse* mouseEvent) override;
	void onMouseUpDetail(SGEventMouse* mouseEvent) override;
	bool onMouseUpContainedDetail(SGEventMouse* mouseEvent) override;

	UIElementType_t getElementType() override { return UIElementType::ToggleButton; }
	SGString toString() override { return SGStringUtil::Format("토글버튼(%d)", m_pInfo->Code); }
private:
	int m_iToggleState;
	UIToggleButtonInfo* m_pInfo;
	FrameTexture* m_pTexture[2][eMax];
	SGSprite* m_pSprite[2][eMax];
};