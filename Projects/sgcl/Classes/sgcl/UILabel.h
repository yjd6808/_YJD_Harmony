/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:53:53 PM
 * =====================
 *
 */


#pragma once


#include <sgcl/UIElement.h>

class UILabel : public UIElement
{
public:
	static UILabel* create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UILabel* create(UIRootGroup* _pRoot, UIGroup* _pParent, UILabelInfo* _pLabelInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::Label; }

	UILabel(UIRootGroup* _pRoot, UIGroup* _pParent);
	UILabel(UIRootGroup* _pRoot, UIGroup* _pParent, UILabelInfo* _pLabelInfo, bool _infoOwner);
	~UILabel() override;

	bool init() override;

	jc::String getFontPath() const;
	const std::string& getString() const { return pLabel_->getString(); }
	void setText(const std::string& _text);
	void setText(const std::string& _text, float _fontSize);
	void setText(const std::string& _text, float _fontSize, const cc::size& _dimension);
	void SetUISize(const cc::size& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoLabel(UILabelInfo* _pInfo, bool _infoOwner);
	void setVAlignment(VAlignment_t _valign);
	void setHAlignment(HAlignment_t _halign);

	void SetInitialUISize(cc::size _size) override;
	void setEnableFontAutoScaling(bool _enableScaling) { fontAutoScaling_ = _enableScaling; }
	bool isFontAutoScalingEnabled() const { return fontAutoScaling_; }

	float getAppliedFontSize() const;
	float getFontSize() const { return fontSize_; }
	float getInitialFontSize() const { return initialFontSize_; }
	int getLineCount() const;

	cc::Label* source() const { return pLabel_; }
	UILabelInfo* getInfo() const { return pInfo_; }
	UIElementType_t GetElementType() override { return UIElementType::Label; }
	jc::String ToString() override { return jc::StringUtil::Format("라벨(%d)", pInfo_->code_); }

private:
	float initialFontSize_;
	float fontSize_;
	bool fontAutoScaling_; // UI 사이즈 변경시 변경된 사이즈 비율에 맞게 폰트 크기도 변경시킬지 여부
	UILabelInfo* pInfo_;
	cc::Label* pLabel_;
};
