/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:33 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGUIElement.h>

class SGUIStatic : public SGUIElement
{
public:
	SGUIStatic(SGUIGroup* parent, SGUIStaticInfo* staticInfo);
	~SGUIStatic() override;

	bool init() override;
	void load() override;
	void unload() override;

	void setDebugVisible(bool visible);
	static SGUIStatic* create(SGUIGroup* parent, SGUIStaticInfo* staticInfo);
	UIElementType_t getElementType() override { return UIElementType::Static; }
private:
	bool m_bVisible;
	SGUIStaticInfo* m_pInfo;
	SGFrameTexture* m_pDebugTexture;
	SGSprite* m_pDebugSprite;
};