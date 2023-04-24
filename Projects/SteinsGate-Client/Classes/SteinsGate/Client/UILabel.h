/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:53:53 PM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/UIElement.h>

class UILabel : public UIElement
{
public:
	UILabel(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo);
	~UILabel() override;

	bool init() override;
	static UILabel* create(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo);
	UIElementType_t getElementType() override { return UIElementType::Sprite; }
	SGString toString() override { return SGStringUtil::Format("라벨(%d)", m_pInfo->Code); }

private:
	UILabelInfo* m_pInfo;
	SGLabel* m_pLabel;
};


