/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:25 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ImagePack.h"

#include <JCore/FileSystem/Path.h>
#include <SteinsGate/Common/SgaSpriteAbstract.h>
#include <SteinsGate/Common/SgaColorFormat.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack::ImagePack(const SgaPackagePtr& _pSgaPackage, int _packIndex)
: index_(_packIndex)
, hasLoadedData_(false)
, package_(_pSgaPackage)
, textureCacheMap_(1'000)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ImagePack::~ImagePack()
{
	clearCache();
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePack::getSpriteCount(int _imgIndex) const
{
	if (!package_->IsElementLoaded(_imgIndex))
	{
		package_->LoadElementIndex(_imgIndex);
	}

	const SgaElementPtr pElement = package_->Get(_imgIndex);
	const SgaImage* pImg = pElement.Get<SgaImage*>();
	return pImg->Count();
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstractPtr ImagePack::getSprite(int _imgIndex, int _frameIndex) const
{
	if (!package_->IsElementLoaded(_imgIndex))
	{
		package_->LoadElementIndex(_imgIndex);
	}

	const SgaElementPtr pElement = package_->Get(_imgIndex);
	DebugAssertMsg(pElement->GetType() == SgaElement::eImage, "엘리먼트가 Img 타입이 아닙니다.");
	return pElement.Get<SgaImage*>()->GetAt(_frameIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpriteAbstractPtr ImagePack::getSpriteUnsafe(int _imgIndex, int _frameIndex) const
{
	if (_imgIndex == InvalidValue_v || _frameIndex == InvalidValue_v)
	{
		return nullptr;
	}

	if (!package_->IsElementLoaded(_imgIndex))
	{
		package_->LoadElementIndex(_imgIndex);
	}

	const SgaElementPtr pElement = package_->GetUnsafe(_imgIndex);

	if (pElement == nullptr)
	{
		return nullptr;
	}

	if (pElement->GetType() == SgaElement::eSound)
	{
		return nullptr;
	}

	DebugAssertMsg(pElement->GetType() == SgaElement::eImage, "엘리먼트가 Img 타입이 아닙니다.");
	SgaImage* pImg = pElement.Get<SgaImage*>();
	return pImg->GetAtUnsafe(_frameIndex);
}

/*
 * 생성한 텍스쳐는 캐싱해놓자.
 * 같은 몬스터 수십마리를 매번 파일스트림에서 텍스쳐 데이터를 읽고 압축 해제하고 32bit 이미지화 시킬 수는 없자나?
 */
//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* ImagePack::createFrameTexture(int _imgIndex, int _frameIndex, bool _linearDodge)
{
	if (_frameIndex == InvalidValue_v)
	{
		return Global::Get()->getDefaultFrameTexture();
	}

	SgaResourceIndex resourceIndex;
	resourceIndex.Un.SgaIndex = index_;
	resourceIndex.Un.ImgIndex = _imgIndex;
	resourceIndex.Un.FrameIndex = _frameIndex;

	if (textureCacheMap_.Exist(resourceIndex.Value))
	{
		FrameTexture* pCached = textureCacheMap_[resourceIndex.Value];
		return pCached;
	}

	if (!package_->IsElementLoaded(_imgIndex))
	{
		package_->LoadElementOnly(_imgIndex);
	}

	SgaImage& image = (SgaImage&)package_->GetAtRef(_imgIndex);

	if (!image.IndexLoaded() && !image.LoadIndexOnly())
	{
		_LogWarn_("%s (Img로딩 실패)", resourceIndex.ToString().Source());
		return Core::Contents.Global->getDefaultFrameTexture();
	}

	if (_frameIndex < 0 || _frameIndex >= image.Count())
	{
		_LogWarn_("%s (올바르지 않은 인덱스 범위)", resourceIndex.ToString().Source());
		DebugAssert(false);
		return Core::Contents.Global->getDefaultFrameTexture();
	}

	SgaSpriteAbstract& sprite = image.GetAtRef(_frameIndex);

	if (sprite.IsLink())
	{
		const int targetFrameIndex = sprite.GetTargetFrameIndex();
		LinkFrameTexture* pLinkTexture = dbg_new LinkFrameTexture(_frameIndex, targetFrameIndex);
		pLinkTexture->autorelease();
		pLinkTexture->retain();
		hasLoadedData_ = true;
		textureCacheMap_.Insert(resourceIndex.Value, pLinkTexture);
		return pLinkTexture;
	}

	if (sprite.IsDummy())
	{
		_LogWarn_("%s (더미 스프라이트)", resourceIndex.ToString().Source());
		return Core::Contents.Global->getDefaultFrameTexture();
	}

	if (!sprite.Loaded())
	{
		sprite.Load();
	}

	const SgaDataPtr pData = sprite.Decompress();

	if (_linearDodge)
	{
		applyLinearDodge(pData.GetPtr(), pData.Length());
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
	textureCacheMap_.Insert(resourceIndex.Value, pSpriteTexture);
#if DebugMode
	Core::Contents.PackManager->increaseCounter();
#endif
	return pSpriteTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::releaseFrameTexture(int _imgIndex, int _frameIndex)
{
	const SgaResourceIndex resourceIndex{ index_, _imgIndex, _frameIndex };
	DebugAssertMsg(textureCacheMap_.Exist(resourceIndex.Value), "%s, 해당하는 프레임 데이터가 없습니다. [1]",
	               resourceIndex.ToString().Source());
	CC_SAFE_RELEASE(textureCacheMap_[resourceIndex.Value]);
	textureCacheMap_.Remove(resourceIndex.Value);
#if DebugMode
	Core::Contents.PackManager->decreaseCounter();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::releaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex)
{
	DebugAssertMsg(textureCacheMap_.Exist(_sgaResourceIndex.Value), "%s, 해당하는 프레임 데이터가 없습니다. [2]",
	               _sgaResourceIndex.ToString().Source());
	CC_SAFE_RELEASE(textureCacheMap_[_sgaResourceIndex.Value]);
	textureCacheMap_.Remove(_sgaResourceIndex.Value);
#if DebugMode
	Core::Contents.PackManager->decreaseCounter();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString ImagePack::getFileName() const
{
	return Path::FileName(getPath());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ImagePack::hasImgIndex(const SGString& _imgName) const
{
	return package_->HasElementIndex(_imgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
int ImagePack::getImgIndexUnsafe(const char* _imgName) const
{
	if (!package_->HasElementIndex(_imgName))
	{
		return InvalidValue_v;
	}

	return package_->GetElementIndex(_imgName);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString ImagePack::getImgName(const int _imgIndex) const
{
	const SgaElementPtr pElement = package_->GetUnsafe(_imgIndex);

	if (pElement == nullptr)
	{
		return { 0 };
	}

	return pElement->GetName();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString ImagePack::getImgNameOrDefault(const int _imgIndex, const SGString& _defaultValue) const
{
	const SgaElementPtr pElement = package_->GetUnsafe(_imgIndex);

	if (pElement == nullptr)
	{
		return _defaultValue;
	}

	return pElement->GetName();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::applyLinearDodge(Byte* _pPixelData, int _len) const
{
	for (int i = 0; i < _len; i += 4)
	{
		const Byte max = Math::Max(_pPixelData[i + 0], Math::Max(_pPixelData[i + 1], _pPixelData[i + 2]));
		const Byte sub = 0xff - max;

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
void ImagePack::unload()
{
	if (!hasLoadedData_)
	{
		return;
	}

	package_->UnloadAllElementData();
	hasLoadedData_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ImagePack::clearCache()
{
	textureCacheMap_.ForEachValue([](FrameTexture* _pTexture)
	{
		//DebugAssertMsg(tex->getReferenceCount() == 1, "레퍼런스 카운트가 1이 아닙니다.");
		CC_SAFE_RELEASE(_pTexture);
		Core::Contents.PackManager->decreaseCounter();
	});
	textureCacheMap_.Clear();
}
