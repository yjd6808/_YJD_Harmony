/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Image.h"
#include "sg/Core/AppConfig.h"
#include "jc/FileSystem/Path.h"
#include "jc/FileSystem/File.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

namespace sgui
{

Image* Image::Create(const char* _sourcePath /* = nullptr */)
{
	Image* pImage = dbg_new Image;
	pImage->autorelease();

	if (_sourcePath)
	{
		pImage->SetSource(_sourcePath);
	}

	return pImage;
}

Image::~Image()
{
	pTexture_ = nullptr;
}

void Image::SetSource(const char* _sourcePath)
{
	sourcePath_ = _sourcePath ? _sourcePath : "";
	RebuildSprite();
	InvalidateLayout();
}

cc::size Image::GetNaturalSize() const
{
	if (pTexture_ == nullptr)
	{
		return cc::size(0.0f, 0.0f);
	}

	return pTexture_->getContentSize();
}

void Image::RebuildSprite()
{
	if (pSprite_)
	{
		pSprite_->removeFromParent();
		pSprite_ = nullptr;
	}

	pTexture_ = nullptr;

	if (sourcePath_.IsEmpty())
	{
		return;
	}

	// resData 기준 상대 경로 변환 (절대 경로는 그대로 사용)
	const bool isAbsolute = sourcePath_.Find(":") != -1
		|| (!sourcePath_.IsEmpty() && (sourcePath_[0] == '/' || sourcePath_[0] == '\\'));
	const jc::String resolved = isAbsolute
		? sourcePath_
		: jc::Path::Combine(g_cAppConfig.resDataPath_, sourcePath_);

	if (!jc::File::Exist(resolved.Source()))
	{
		_LogWarn_("[Image] 텍스처 파일이 존재하지 않습니다: %s", resolved.Source());
		return;
	}

	pTexture_ = Director::getInstance()->getTextureCache()->addImage(resolved.Source());

	if (pTexture_ == nullptr)
	{
		_LogWarn_("[Image] 텍스처 로드 실패: %s", resolved.Source());
		return;
	}

	pSprite_ = Scale9Sprite::create();
	pSprite_->initWithTexture(pTexture_);
	pSprite_->setAnchorPoint(cc::vec2::ZERO);
	addChild(pSprite_);
}

cc::size Image::MeasureOverride(const cc::size& _constraint)
{
	const cc::size natural = GetNaturalSize();

	if (stretch_ == Stretch::None || std::isinf(_constraint.width) || std::isinf(_constraint.height))
	{
		return natural;
	}

	return cc::size(
		std::min(natural.width, _constraint.width),
		std::min(natural.height, _constraint.height));
}

void Image::ArrangeOverride(const cc::size& _finalSize)
{
	if (pSprite_ == nullptr)
	{
		return;
	}

	const cc::size natural = GetNaturalSize();

	switch (stretch_)
	{
	case Stretch::None:
	{
		pSprite_->setContentSize(natural);
		pSprite_->setPosition(
			(_finalSize.width - natural.width) * 0.5f,
			(_finalSize.height - natural.height) * 0.5f);
		break;
	}
	case Stretch::Uniform:
	case Stretch::UniformToFill:
	{
		float scale = 1.0f;

		if (natural.width > 0.0f && natural.height > 0.0f)
		{
			const float scaleX = _finalSize.width / natural.width;
			const float scaleY = _finalSize.height / natural.height;
			scale = stretch_ == Stretch::Uniform ? std::min(scaleX, scaleY) : std::max(scaleX, scaleY);
		}

		const cc::size scaled(natural.width * scale, natural.height * scale);
		pSprite_->setContentSize(scaled);
		pSprite_->setPosition(
			(_finalSize.width - scaled.width) * 0.5f,
			(_finalSize.height - scaled.height) * 0.5f);
		break;
	}
	case Stretch::Fill:
	default:
		pSprite_->setContentSize(_finalSize);
		pSprite_->setPosition(0.0f, 0.0f);
		break;
	}
}

} // namespace sgui
