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
	static UISprite* create(UIMasterGroup* master, UIGroup* parent);
	static UISprite* create(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* spriteInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Sprite; }

	UISprite(UIMasterGroup* master, UIGroup* parent);
	UISprite(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* staticInfo, bool infoOwner);
	~UISprite() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setCapInsets(const SGRect& insets);
	void setUISize(const SGSize& contentSize) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoSprite(UISpriteInfo* info, bool infoOwner);
	void setOpacity(GLubyte opacity) override;
	bool isScale9() const { return m_pInfo->Scale9; }

	UISpriteInfo* getInfo() const { return m_pInfo; }
	SGSprite* source() const { return m_pSprite; }
	UIElementType_t getElementType() override { return UIElementType::Sprite; }
	SGString toString() override { return SGStringUtil::Format("스프라이트(%d)", m_pInfo->Code); }
private:
	UISpriteInfo* m_pInfo;
	FrameTexture* m_pTexture;
	SGSprite* m_pSprite;
};