/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:42:12 PM
 * =====================
 *
 */

#include "Core.h"
#include <sg/SgaImage.h>
#include <sg/SgaSprite.h>
#include <sg/SgaElementInitializer.h>

#include "SgaPackage.h"

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
SgaImage::SgaImage(const Header& _header, int _size)
: SgaElement(SgaElement::eImage, _header)
, sprite_(_size, nullptr)
, waitForLoadingDataCount_(_size)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaImage::~SgaImage()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaImagePtr SgaImage::Create(const Header& _header, int _size)
{
	return MakeShared<SgaImage>(_header, _size);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstract& SgaImage::operator[](const int _idx) const
{
	jc_assert(IsValidIndex(_idx));
	return sprite_[_idx].GetRef();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaImage::IsValidIndex(int _index) const
{
	return _index >= 0 && _index < sprite_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaImage::Load(bool _indexOnly)
{
	if (!pParent_.Exist())
		return false;

	SgaElementInitializer::InitializeElement(
		Shared(),
		pParent_->StreamRef(),
		_indexOnly
	);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaImage::Unload()
{
	bool hasUnloadedData = false;

	for (int i = 0; i < sprite_.Size(); ++i)
	{
		if (sprite_[i].GetRef().Unload())
		{
			hasUnloadedData = true;
		}
	}

	return hasUnloadedData;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String SgaImage::ToString() const
{
	return StringUtil::Format("Sga(%s) Img(%s)", pParent_->GetPath().Source(), header_.name_.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstractPtr SgaImage::GetAt(const int _idx) const
{
	jc_assert(IsValidIndex(_idx));
	return sprite_[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstractPtr SgaImage::GetAtUnsafe(const int _idx) const
{
	if (!IsValidIndex(_idx))
		return nullptr;

	return sprite_[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstract& SgaImage::GetAtRef(const int _idx) const
{
	jc_assert(IsValidIndex(_idx));
	return sprite_[_idx].GetRef();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SgaImage::Set(int _idx, const SgaSpriteAbstractPtr& _pSprite) const
{
	jc_assert_msg(sprite_[_idx] == nullptr, "이미 초기화된 스프라이트 인덱스에 넣을려고 하고 있습니다.");
	sprite_[_idx] = _pSprite;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SgaImage::Remove(const SgaSpriteAbstractPtr& _pSprite)
{
	for (int i = 0; i < sprite_.Size(); ++i)
	{
		if (sprite_[i] == _pSprite)
		{
			sprite_.RemoveAt(i--);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SgaImage::RemoveAt(const int _idx)
{
	sprite_.RemoveAt(_idx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SgaImage::Clear()
{
	sprite_.Clear();
}
