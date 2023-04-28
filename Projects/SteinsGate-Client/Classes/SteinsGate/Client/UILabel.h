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
	static UILabel* create(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo);

	UILabel(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo);
	~UILabel() override;

	bool init() override;

	
	SGString getFontPath() const;
	void setText(const std::string& text);
	void setText(const std::string& text, float width, float height);
	void setContentSize(const SGSize& contentSize) override;

	SGLabel* source() const { return m_pLabel; }
	UILabelInfo* getInfo() const { return m_pInfo; }
	UIElementType_t getElementType() override { return UIElementType::Label; }
	SGString toString() override { return SGStringUtil::Format("라벨(%d)", m_pInfo->Code); }
private:
	UILabelInfo* m_pInfo;
	SGLabel* m_pLabel;
};


