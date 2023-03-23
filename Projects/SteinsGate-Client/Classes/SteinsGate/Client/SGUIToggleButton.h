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
	static constexpr int StateOne = 0;
	static constexpr int StateTwo = 1;
public:
	SGUIToggleButton(SGUIMasterGroup* master, SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo);
	~SGUIToggleButton() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setVisibleState(State state);
	void setEnabled(bool enabled) override;
	int getToggleState() const { return m_iToggleState; }

	static SGUIToggleButton* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo);
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
	SGUIToggleButtonInfo* m_pInfo;
	SGFrameTexture* m_pTexture[2][eMax];
	SGSprite* m_pSprite[2][eMax];
};