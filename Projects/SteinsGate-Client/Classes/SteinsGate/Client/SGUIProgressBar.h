/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUIProgressBar : public SGUIElement
{
public:
	SGUIProgressBar(SGUIMasterGroup* master, SGUIGroup* parent, SGUIProgressBarInfo* progressBarInfo);
	~SGUIProgressBar() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setPercent(float percent) const;
	float getPercent() const;

	static SGUIProgressBar* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIProgressBarInfo* progressBarInfo);
	UIElementType_t getElementType() override { return UIElementType::ProgressBar; }
	SGString toString() override { return SGStringUtil::Format("프로그래스바(%d)", m_pInfo->Code); }
private:
	SGUIProgressBarInfo* m_pInfo;
	SGFrameTexture* m_pTexture;
	SGProgressTimer* m_pProgressBar;
	SGSprite* m_pProgressSprite;
};