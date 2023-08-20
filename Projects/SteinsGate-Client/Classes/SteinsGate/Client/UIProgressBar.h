/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UIProgressBar : public UIElement
{
public:
	static UIProgressBar* create(UIMasterGroup* master, UIGroup* parent);
	static UIProgressBar* create(UIMasterGroup* master, UIGroup* parent, UIProgressBarInfo* progressBarInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::ProgressBar; }

	UIProgressBar(UIMasterGroup* master, UIGroup* parent);
	UIProgressBar(UIMasterGroup* master, UIGroup* parent, UIProgressBarInfo* progressBarInfo, bool infoOwner);
	~UIProgressBar() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setUISize(const SGSize& size) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoProgressBar(UIProgressBarInfo* info, bool infoOwner);
	void setPercent(float percent) const;
	float getPercent() const;

	SGProgressTimer* source() const { return m_pProgressBar; }
	UIElementType_t getElementType() override { return UIElementType::ProgressBar; }
	SGString toString() override { return SGStringUtil::Format("프로그래스바(%d)", m_pInfo->Code); }
private:
	UIProgressBarInfo* m_pInfo;
	FrameTexture* m_pTexture;
	SGProgressTimer* m_pProgressBar;
	SGSprite* m_pProgressSprite;
};