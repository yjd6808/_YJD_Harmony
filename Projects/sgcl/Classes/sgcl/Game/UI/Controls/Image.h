/*
 * 작성자: 윤정도
 * =====================
 * sgui - 이미지 표시 요소 (WPF Image)
 * Source는 resData 기준 텍스처 파일 경로다. (SGA 리소스 키 사용 금지)
 */

#pragma once

#include "sgcl/Game/UI/Core/UIElement.h"

namespace sgui
{

class Image : public UIElement
{
public:
	static Image* Create(const char* _sourcePath = nullptr);
	virtual ~Image() override;

	void SetSource(const char* _sourcePath);
	const std::string& GetSource() const { return sourcePath_; }

	void SetStretch(Stretch _stretch) { stretch_ = _stretch; InvalidateLayout(); }
	Stretch GetStretch() const { return stretch_; }

	cc::size GetNaturalSize() const;

protected:
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	void RebuildSprite();

	std::string sourcePath_;
	Stretch stretch_ = Stretch::Fill;

	cc_ui::Scale9Sprite* pSprite_ = nullptr;
	cc::Texture2D* pTexture_ = nullptr;
};

} // namespace sgui
