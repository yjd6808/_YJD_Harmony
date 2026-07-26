#pragma once

#include "sgcl/Game/UI/UIElement.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemeBinding.h"

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

	void SetUISize(const cc::size& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoProgressBar(UIProgressBarInfo* _pInfo, bool _infoOwner);
	void SetPercent(float _percent) const;
	float GetPercent() const;

	UITextureMode GetTextureMode() const { return textureMode_; }
	void RefreshThemeVisuals();

	cc::ProgressTimer* Source() const { return pProgressBar_; }
	UIElementType_t GetElementType() override { return UIElementType::ProgressBar; }
	jc::String ToString() override { return jc::StringUtil::Format("프로그래스바(%s)", pProgressBarInfo_->name_); }

protected:
	void LoadTheme();
	bool LoadLegacy();
	void BuildThemeVisuals();
	void DestroyThemeVisuals();
	void UpdateGaugeGeometry();

private:
	UIProgressBarInfo* pProgressBarInfo_;
	FrameTexture* pTexture_;
	cc::ProgressTimer* pProgressBar_;
	cc::Sprite* pProgressSprite_;

	UITextureMode textureMode_ = UITextureMode::NONE;
	UIThemeTextureBinding themeBinding_;
	uint64_t appliedTextureRevision_ = 0;

	cc::Size gaugeMinSize_ = { 12, 12 };

	cc_ui::Scale9Sprite* pTrackSprite_ = nullptr;
	cc_ui::Scale9Sprite* pGaugeSprite_ = nullptr;
	cc::Sprite* pGaugeCap_ = nullptr;
	float percent_ = 0.0f;
};
