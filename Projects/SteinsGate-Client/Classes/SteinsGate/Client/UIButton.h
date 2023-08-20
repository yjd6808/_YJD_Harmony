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
	static UIButton* create(UIMasterGroup* master, UIGroup* parent);
	static UIButton* create(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Button; }

	UIButton(UIMasterGroup* master, UIGroup* parent);
	UIButton(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo, bool infoOwner);
	~UIButton() override;
	 
	bool init() override;
	void load() override;
	void unload() override;
	
	void setVisibleState(State state);
	void setUISize(const SGSize& size) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoButton(UIButtonInfo* info, bool infoOwner);
	void setEnabled(bool enabled) override;
	void restoreState(State state) override;
	UIElementType_t getElementType() override { return UIElementType::Button; }
	SGString toString() override { return SGStringUtil::Format("버튼(%d)", m_pInfo->Code); }
protected:
	void onMouseEnterInternalDetail(SGEventMouse* mouseEvent) override;
	void onMouseLeaveInternalDetail(SGEventMouse* mouseEvent) override;
	bool onMouseMoveInternalDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownInternalDetail(SGEventMouse* mouseEvent) override;
	void onMouseUpInternalDetail(SGEventMouse* mouseEvent) override;
private:
	UIButtonInfo* m_pInfo;
	FrameTexture* m_pTexture[eMax];
	SGSprite* m_pSprite[eMax];
};
