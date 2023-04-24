/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:33 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/UIElement.h>

class UIStatic : public UIElement
{
public:
	UIStatic(UIMasterGroup* master, UIGroup* parent, UIStaticInfo* staticInfo);
	~UIStatic() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setDebugVisible(bool visible);
	static UIStatic* create(UIMasterGroup* master, UIGroup* parent, UIStaticInfo* staticInfo);
	UIElementType_t getElementType() override { return UIElementType::Static; }
	SGString toString() override { return SGStringUtil::Format("스태틱(%d)", m_pInfo->Code); }
private:
	bool m_bVisible;
	UIStaticInfo* m_pInfo;
	FrameTexture* m_pDebugTexture;
	SGSprite* m_pDebugSprite;
};