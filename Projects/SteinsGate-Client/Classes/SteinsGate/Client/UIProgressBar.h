/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

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

	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoProgressBar(UIProgressBarInfo* _pInfo, bool _infoOwner);
	void SetPercent(float _percent) const;
	float GetPercent() const;

	SGProgressTimer* Source() const { return pProgressBar_; }
	UIElementType_t GetElementType() override { return UIElementType::ProgressBar; }
	SGString ToString() override { return SGStringUtil::Format("프로그래스바(%d)", pProgressBarInfo_->code_); }

private:
	UIProgressBarInfo* pProgressBarInfo_;
	FrameTexture* pTexture_;
	SGProgressTimer* pProgressBar_;
	SGSprite* pProgressSprite_;
};
