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
	SGUISprite(SGUIMasterGroup* master, SGUIGroup* parent, SGUISpriteInfo* staticInfo);
	~SGUISprite() override;

	bool init() override;
	void load() override;
	void unload() override;

	static SGUISprite* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUISpriteInfo* spriteInfo);
	UIElementType_t getElementType() override { return UIElementType::Sprite; }
	SGString toString() override { return SGStringUtil::Format("스프라이트(%d)", m_pInfo->Code); }
private:
	SGUISpriteInfo* m_pInfo;
	SGFrameTexture* m_pTexture;
	SGSprite* m_pSprite;
};