/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:15 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UISprite : public UIElement
{
public:
	UISprite(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* staticInfo);
	~UISprite() override;

	bool init() override;
	void load() override;
	void unload() override;

	static UISprite* create(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* spriteInfo);
	UIElementType_t getElementType() override { return UIElementType::Sprite; }
	SGString toString() override { return SGStringUtil::Format("스프라이트(%d)", m_pInfo->Code); }
private:
	UISpriteInfo* m_pInfo;
	FrameTexture* m_pTexture;
	SGSprite* m_pSprite;
};