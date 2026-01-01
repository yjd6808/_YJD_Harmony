/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:01 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/UIElement.h>

class UIProgressBar : public UIElement
{
public:
	static UIProgressBar* Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	static UIProgressBar* Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UIProgressBarInfo* _pProgressBarInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::ProgressBar; }

	UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UIProgressBarInfo* _pProgressBarInfo, bool _infoOwner);
	~UIProgressBar() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetUISize(const c2d::size& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoProgressBar(UIProgressBarInfo* _pInfo, bool _infoOwner);
	void SetPercent(float _percent) const;
	float GetPercent() const;

	c2d::ProgressTimer* Source() const { return pProgressBar_; }
	UIElementType_t GetElementType() override { return UIElementType::ProgressBar; }
	jc::String ToString() override { return jc::StringUtil::Format("프로그래스바(%d)", pProgressBarInfo_->code_); }

private:
	UIProgressBarInfo* pProgressBarInfo_;
	FrameTexture* pTexture_;
	c2d::ProgressTimer* pProgressBar_;
	c2d::Sprite* pProgressSprite_;
};
