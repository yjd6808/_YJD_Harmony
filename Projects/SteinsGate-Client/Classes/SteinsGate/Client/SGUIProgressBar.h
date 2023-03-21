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
	SGUIProgressBar(SGUIGroup* parent, SGUIProgressBarInfo* progressBarInfo);
	~SGUIProgressBar() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setPercent(float percent) const;
	float getPercent() const;

	static SGUIProgressBar* create(SGUIGroup* parent, SGUIProgressBarInfo* progressBarInfo);
	UIElementType_t getElementType() override { return UIElementType::ProgressBar; }
private:
	SGUIProgressBarInfo* m_pInfo;
	SGFrameTexture* m_pTexture;
	SGProgressBar* m_pProgressBar;
	SGSprite* m_pProgressSprite;
};