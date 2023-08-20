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
	static UIStatic* create(UIMasterGroup* master, UIGroup* parent);
	static UIStatic* create(UIMasterGroup* master, UIGroup* parent, UIStaticInfo* staticInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Static; }

	UIStatic(UIMasterGroup* master, UIGroup* parent);
	UIStatic(UIMasterGroup* master, UIGroup* parent, UIStaticInfo* staticInfo, bool infoOwner);
	~UIStatic() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setDebugVisible(bool visible);
	void setUISize(const SGSize& contentSize) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoStatic(UIStaticInfo* info, bool infoOwner);

	UIStaticInfo* getInfo() const { return m_pInfo; }
	UIElementType_t getElementType() override { return UIElementType::Static; }
	SGString toString() override { return SGStringUtil::Format("스태틱(%d)", m_pInfo->Code); }
private:
	bool m_bVisible;
	UIStaticInfo* m_pInfo;
	FrameTexture* m_pDebugTexture;
	SGSprite* m_pDebugSprite;
};