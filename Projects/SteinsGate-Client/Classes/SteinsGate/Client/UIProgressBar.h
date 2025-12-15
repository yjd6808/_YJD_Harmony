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
	static UIProgressBar* create(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	static UIProgressBar* create(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UIProgressBarInfo* _pProgressBarInfo,
	                             bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::ProgressBar; }

	UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UIProgressBarInfo* _pProgressBarInfo,
	              bool _infoOwner);
	~UIProgressBar() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoProgressBar(UIProgressBarInfo* _pInfo, bool _infoOwner);
	void setPercent(float _percent) const;
	float getPercent() const;

	SGProgressTimer* source() const { return progressBar_; }
	UIElementType_t GetElementType() override { return UIElementType::ProgressBar; }
	SGString ToString() override { return SGStringUtil::Format("프로그래스바(%d)", progressBarInfo_->code_); }

private:
	UIProgressBarInfo* progressBarInfo_;
	FrameTexture* texture_;
	SGProgressTimer* progressBar_;
	SGSprite* progressSprite_;
};
