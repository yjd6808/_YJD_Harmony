/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:15 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUISprite : public SGUIElement
{
public:
	SGUISprite(SGUIGroup* parent, SGUISpriteInfo* btnInfo);
	~SGUISprite() override;

	bool init() override;
	void load() override;
	void unload() override;

	static SGUISprite* create(SGUIGroup* parent, SGUISpriteInfo* spriteInfo);
	UIElementType_t getElementType() override { return UIElementType::Sprite; }
private:
	SGUISpriteInfo* m_pInfo;
	SGFrameTexture* m_pTexture;
	SGSprite* m_pSprite;
};