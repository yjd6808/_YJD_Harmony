/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:53:53 PM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGUIElement.h>

class SGUILabel : public SGUIElement
{
public:
	SGUILabel(SGUIGroup* parent, SGUILabelInfo* btnInfo);
	~SGUILabel();

	bool init() override;
	static SGUILabel* create(SGUIGroup* parent, SGUILabelInfo* labelInfo);
	int getCode() override;
	UIElementType_t getElementType() override { return UIElementType::Sprite; }

private:
	SGUILabelInfo* m_pInfo;
	SGLabel* m_pLabel;
};


