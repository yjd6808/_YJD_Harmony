/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:25 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "ImagePack.h"

#include <jc/FileSystem/Path.h>
#include <sg/_Sga/SgaSpriteAbstract.h>
#include <sg/_Sga/SgaColorFormat.h>

#include <sgcl/ImagePackManager.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack::ImagePack(const SgaPackagePtr& _pSgaPackage, int _packIndex)
: index_(_packIndex)
, hasLoadedData_(false)
, pPackage_(_pSgaPackage)
, textureCacheMap_(1'000)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack::~ImagePack()
{
	ClearCache();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePack::GetSpriteCount(int _imgIndex) const
{
	if (!pPackage_->IsElementLoaded(_imgIndex))
	{
		pPackage_->LoadElementIndex(_imgIndex);
	}

	const SgaElementPtr pElement = pPackage_->Get(_imgIndex);
	const SgaImage* pImg = pElement.Get<SgaImage*>();
	return pImg->Count();
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstractPtr ImagePack::GetSprite(int _imgIndex, int _frameIndex) const
{
	if (!pPackage_->IsElementLoaded(_imgIndex))
	{
		pPackage_->LoadElementIndex(_imgIndex);
	}

	const SgaElementPtr pElement = pPackage_->Get(_imgIndex);
	jc_assert_msg(pElement->GetType() == SgaElement::eImage, "엘리먼트가 Img 타입이 아닙니다.");
	return pElement.Get<SgaImage*>()->GetAt(_frameIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstractPtr ImagePack::GetSpriteUnsafe(int _imgIndex, int _frameIndex) const
{
	if (_imgIndex == InvalidValue_v || _frameIndex == InvalidValue_v)
	{
		return nullptr;
	}

	if (!pPackage_->IsElementLoaded(_imgIndex))
	{
		pPackage_->LoadElementIndex(_imgIndex);
	}

	const SgaElementPtr pElement = pPackage_->GetUnsafe(_imgIndex);

	if (pElement == nullptr)
	{
		return nullptr;
	}

	if (pElement->GetType() == SgaElement::eSound)
	{
		return nullptr;
	}

	jc_assert_msg(pElement->GetType() == SgaElement::eImage, "엘리먼트가 Img 타입이 아닙니다.");
	SgaImage* pImg = pElement.Get<SgaImage*>();
	return pImg->GetAtUnsafe(_frameIndex);
}

/*
 * 생성한 텍스쳐는 캐싱해놓자.
 * 같은 몬스터 수십마리를 매번 파일스트림에서 텍스쳐 데이터를 읽고 압축 해제하고 32bit 이미지화 시킬 수는 없자나?
 */
//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* ImagePack::CreateFrameTexture(int _imgIndex, int _frameIndex, bool _linearDodge)
{
	if (_frameIndex == InvalidValue_v)
	{
		return SpriteFrameTexture::GetDefault();
	}

	SgaResourceIndex resourceIndex;
	resourceIndex.un_.sgaIndex_ = index_;
	resourceIndex.un_.imgIndex_ = _imgIndex;
	resourceIndex.un_.frameIndex_ = _frameIndex;

	if (textureCacheMap_.Exist(resourceIndex.value_))
	{
		FrameTexture* pCached = textureCacheMap_[resourceIndex.value_];
		return pCached;
	}

	if (!pPackage_->IsElementLoaded(_imgIndex))
	{
		pPackage_->LoadElementOnly(_imgIndex);
	}

	SgaImage& image = (SgaImage&)pPackage_->GetAtRef(_imgIndex);

	if (!image.IndexLoaded() && !image.LoadIndexOnly())
	{
		_LogWarn_("%s (Img로딩 실패)", resourceIndex.ToString().Source());
		return SpriteFrameTexture::GetDefault();
	}

	if (_frameIndex < 0 || _frameIndex >= image.Count())
	{
		_LogWarn_("%s (올바르지 않은 인덱스 범위)", resourceIndex.ToString().Source());
		jc_assert(false);
		return SpriteFrameTexture::GetDefault();
	}

	SgaSpriteAbstract& sprite = image.GetAtRef(_frameIndex);

	if (sprite.IsLink())
	{
		const int targetFrameIndex = sprite.GetTargetFrameIndex();
		LinkFrameTexture* pLinkTexture = dbg_new LinkFrameTexture(_frameIndex, targetFrameIndex);
		pLinkTexture->autorelease();
		pLinkTexture->retain();
		hasLoadedData_ = true;
		textureCacheMap_.Insert(resourceIndex.value_, pLinkTexture);
		return pLinkTexture;
	}

	if (sprite.IsDummy())
	{
		_LogWarn_("%s (더미 스프라이트)", resourceIndex.ToString().Source());
		return SpriteFrameTexture::GetDefault();
	}

	if (!sprite.Loaded())
	{
		sprite.Load();
	}

	const SgaDataPtr pData = sprite.Decompress();

	if (_linearDodge)
	{
		ApplyLinearDodge(pData.GetPtr(), pData.Length());
	}

	Texture2D* pTexture = dbg_new Texture2D;
	pTexture->initWithData(
		pData.GetPtr(),
		pData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(), { sprite.GetWidthF(), sprite.GetHeightF() }
	);

	auto pSpriteTexture = dbg_new SpriteFrameTexture(pTexture, sprite.GetRect(), sprite.GetFrameIndex(),
	                                                 sprite.IsDummy(), _linearDodge);
	pSpriteTexture->autorelease();
	pSpriteTexture->retain(); // textureCacheMap_에서 수명연장시키기 위한 용도
	hasLoadedData_ = true;
	textureCacheMap_.Insert(resourceIndex.value_, pSpriteTexture);
#if _DEBUG
	g_cImagePackMgr.IncreaseCounter();
#endif
	return pSpriteTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::ReleaseFrameTexture(int _imgIndex, int _frameIndex)
{
	const SgaResourceIndex resourceIndex{ index_, _imgIndex, _frameIndex };
	jc_assert_msg(textureCacheMap_.Exist(resourceIndex.value_), "%s, 해당하는 프레임 데이터가 없습니다. [1]",
	               resourceIndex.ToString().Source());
	CC_SAFE_RELEASE(textureCacheMap_[resourceIndex.value_]);
	textureCacheMap_.Remove(resourceIndex.value_);
#if _DEBUG
	g_cImagePackMgr.DecreaseCounter();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::ReleaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex)
{
	jc_assert_msg(textureCacheMap_.Exist(_sgaResourceIndex.value_), "%s, 해당하는 프레임 데이터가 없습니다. [2]",
	               _sgaResourceIndex.ToString().Source());
	CC_SAFE_RELEASE(textureCacheMap_[_sgaResourceIndex.value_]);
	textureCacheMap_.Remove(_sgaResourceIndex.value_);
#if _DEBUG
	g_cImagePackMgr.DecreaseCounter();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String ImagePack::GetFileName() const
{
	return Path::FileName(GetPath());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ImagePack::HasImgIndex(const jc::String& _imgName) const
{
	return pPackage_->HasElementIndex(_imgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePack::GetImgIndexUnsafe(const char* _imgName) const
{
	if (!pPackage_->HasElementIndex(_imgName))
	{
		return InvalidValue_v;
	}

	return pPackage_->GetElementIndex(_imgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String ImagePack::GetImgName(const int _imgIndex) const
{
	const SgaElementPtr pElement = pPackage_->GetUnsafe(_imgIndex);

	if (pElement == nullptr)
	{
		return jc::String( 0 );
	}

	return pElement->GetName();
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String ImagePack::GetImgNameOrDefault(const int _imgIndex, const jc::String& _defaultValue) const
{
	const SgaElementPtr pElement = pPackage_->GetUnsafe(_imgIndex);

	if (pElement == nullptr)
	{
		return _defaultValue;
	}

	return pElement->GetName();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::ApplyLinearDodge(_u8* _pPixelData, int _len) const
{
	for (int i = 0; i < _len; i += 4)
	{
		const _u8 max = Math::Max(_pPixelData[i + 0], Math::Max(_pPixelData[i + 1], _pPixelData[i + 2]));
		const _u8 sub = 0xff - max;

		/* a */
		_pPixelData[i + 3] = Math::Min(_pPixelData[i + 3], max);
		/* b */
		_pPixelData[i + 2] += sub;
		/* g */
		_pPixelData[i + 1] += sub;
		/* r */
		_pPixelData[i + 0] += sub;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::Unload()
{
	if (!hasLoadedData_)
	{
		return;
	}

	pPackage_->UnloadAllElementData();
	hasLoadedData_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::ClearCache()
{
	textureCacheMap_.ForEachValue([](FrameTexture* _pTexture)
	{
		//jc_assert_msg(tex->getReferenceCount() == 1, "레퍼런스 카운트가 1이 아닙니다.");
		CC_SAFE_RELEASE(_pTexture);
		g_cImagePackMgr.DecreaseCounter();
	});
	textureCacheMap_.Clear();
}
