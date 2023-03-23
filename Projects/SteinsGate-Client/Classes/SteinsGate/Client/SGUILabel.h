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
	SGUILabel(SGUIMasterGroup* master, SGUIGroup* parent, SGUILabelInfo* labelInfo);
	~SGUILabel() override;

	bool init() override;
	static SGUILabel* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUILabelInfo* labelInfo);
	UIElementType_t getElementType() override { return UIElementType::Sprite; }
	SGString toString() override { return SGStringUtil::Format("라벨(%d)", m_pInfo->Code); }

private:
	SGUILabelInfo* m_pInfo;
	SGLabel* m_pLabel;
};


