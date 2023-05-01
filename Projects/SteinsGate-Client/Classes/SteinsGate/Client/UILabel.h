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
	static constexpr UIElementType_t type() { return UIElementType::Label; }

	UILabel(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo);
	~UILabel() override;

	bool init() override;

	SGString getFontPath() const;
	const std::string& getString() const { return m_pLabel->getString(); }
	void setText(const std::string& text);
	void setText(const std::string& text, float fontSize);
	void setText(const std::string& text, float fontSize, const SGSize& dimesion);
	void setUISize(const SGSize& contentSize) override;
	void setVAlignment(VAlignment_t valign);
	void setHAlignment(HAlignment_t halign);

	void setEnableFontAutoScaling(bool enableScaling) { m_bFontAutoScaling = enableScaling; }
	bool isFontAutoScalingEnabled() const { return m_bFontAutoScaling; }

	float getFontSize() const;
	int getLineCount() const;

	

	SGLabel* source() const { return m_pLabel; }
	UILabelInfo* getInfo() const { return m_pInfo; }
	UIElementType_t getElementType() override { return UIElementType::Label; }
	SGString toString() override { return SGStringUtil::Format("라벨(%d)", m_pInfo->Code); }
private:
	bool m_bFontAutoScaling;	// UI 사이즈 변경시 변경된 사이즈 비율에 맞게 폰트 크기도 변경시킬지 여부
	UILabelInfo* m_pInfo;
	SGLabel* m_pLabel;
};


