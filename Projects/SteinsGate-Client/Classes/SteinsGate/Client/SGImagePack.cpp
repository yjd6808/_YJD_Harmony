/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:25 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGImagePack.h"

#include <JCore/FileSystem/Path.h>
#include <SteinsGate/Common/SgaSpriteAbstract.h>
#include <SteinsGate/Common/SgaColorFormat.h>
#include <SteinsGate/Client/SGGlobal.h>

USING_NS_CC;
USING_NS_JC;

SGImagePack::SGImagePack(const SgaPackagePtr& sgaPackage, int packIndex)
	: m_iIndex(packIndex)
	, m_bHasLoadedData(false)
	, m_Package(sgaPackage)
	, m_TextureCacheMap(1'000) {}

SGImagePack::~SGImagePack() {
	clearCache();
}

int SGImagePack::getSpriteCount(int imgIndex) const {
	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndex(imgIndex);

	const SgaElementPtr spElement = m_Package->Get(imgIndex);
	const SgaImage* pImg = spElement.Get<SgaImage*>();
	return pImg->Count();
}

SgaSpriteAbstractPtr SGImagePack::getSprite(int imgIndex, int frameIndex) const {

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndex(imgIndex);

	const SgaElementPtr spElement = m_Package->Get(imgIndex);
	DebugAssertMsg(spElement->GetType() == SgaElement::eImage, "엘리먼트가 Img 타입이 아닙니다.");
	return spElement.Get<SgaImage*>()->GetAt(frameIndex);
}

SgaSpriteAbstractPtr SGImagePack::getSpriteUnsafe(int imgIndex, int frameIndex) const {

	if (imgIndex == InvalidValue_v || frameIndex == InvalidValue_v)
		return nullptr;

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndex(imgIndex);

	const SgaElementPtr spElement = m_Package->GetUnsafe(imgIndex);

	if (spElement == nullptr)
		return nullptr;

	if (spElement->GetType() == SgaElement::eSound)
		return nullptr;

	DebugAssertMsg(spElement->GetType() == SgaElement::eImage, "엘리먼트가 Img 타입이 아닙니다.");
	SgaImage* pImg = spElement.Get<SgaImage*>();
	return pImg->GetAtUnsafe(frameIndex);
}

/*
 * 생성한 텍스쳐는 캐싱해놓자.
 * 같은 몬스터 수십마리를 매번 파일스트림에서 텍스쳐 데이터를 읽고 압축 해제하고 32bit 이미지화 시킬 수는 없자나?
 */
SGFrameTexture* SGImagePack::createFrameTexture(int imgIndex, int frameIndex, bool linearDodge) {

	if (frameIndex == InvalidValue_v) {
		return SGGlobal::get()->getDefaultFrameTexture();
	}

	SgaResourceIndex index;
	index.Un.SgaIndex = m_iIndex;
	index.Un.ImgIndex = imgIndex;
	index.Un.FrameIndex = frameIndex;

	if (m_TextureCacheMap.Exist(index.Value)) {
		SGFrameTexture* pCached = m_TextureCacheMap[index.Value];
		return pCached;
	}

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementOnly(imgIndex);

	SgaImage& img = (SgaImage&)m_Package->GetAtRef(imgIndex);

	if (!img.IndexLoaded() && !img.LoadIndexOnly()) {
		_LogWarn_("%s (Img로딩 실패)", index.ToString().Source());
		return CoreGlobal_v->getDefaultFrameTexture();
	}

	if (frameIndex < 0 || frameIndex >= img.Count()) {
		_LogWarn_("%s (올바르지 않은 인덱스 범위)", index.ToString().Source());
		DebugAssert(false);
		return CoreGlobal_v->getDefaultFrameTexture();
	}

	SgaSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	if (sprite.IsLink()) {
		const int iTargetFrameIndex = sprite.GetTargetFrameIndex();
		SGLinkFrameTexture* pLinkTexture = dbg_new SGLinkFrameTexture(frameIndex, iTargetFrameIndex);
		pLinkTexture->autorelease();
		pLinkTexture->retain();
		m_bHasLoadedData = true;
		m_TextureCacheMap.Insert(index.Value, pLinkTexture);
		return pLinkTexture;
	}

	if (sprite.IsDummy()) {
		_LogWarn_("%s (더미 스프라이트)", index.ToString().Source());
		return CoreGlobal_v->getDefaultFrameTexture();
	}

	if (!sprite.Loaded())
		sprite.Load();

	const SgaDataPtr spData = sprite.Decompress();

	if (linearDodge)
		applyLinearDodge(spData.GetPtr(), spData.Length());


	Texture2D* pTexture = dbg_new Texture2D;
	pTexture->initWithData(
		spData.GetPtr(),
		spData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(), { sprite.GetWidthF(), sprite.GetHeightF() }
	);

	auto pSpriteTexture = dbg_new SGSpriteFrameTexture(pTexture, sprite.GetRect(), sprite.GetFrameIndex(), sprite.IsDummy(), linearDodge);
	pSpriteTexture->autorelease();
	pSpriteTexture->retain();	// m_TextureCacheMap에서 수명연장시키기 위한 용도
	m_bHasLoadedData = true;
	m_TextureCacheMap.Insert(index.Value, pSpriteTexture);
	return pSpriteTexture;
}

void SGImagePack::releaseFrameTexture(int imgIndex, int frameIndex) {
	const SgaResourceIndex index{ m_iIndex, imgIndex, frameIndex };
	DebugAssertMsg(m_TextureCacheMap.Exist(index.Value), "%s, 해당하는 프레임 데이터가 없습니다. [1]", index.ToString().Source());
	CC_SAFE_RELEASE(m_TextureCacheMap[index.Value]);
	m_TextureCacheMap.Remove(index.Value);
}

void SGImagePack::releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex) {
	DebugAssertMsg(m_TextureCacheMap.Exist(sgaResourceIndex.Value), "%s, 해당하는 프레임 데이터가 없습니다. [2]", sgaResourceIndex.ToString().Source());
	CC_SAFE_RELEASE(m_TextureCacheMap[sgaResourceIndex.Value]);
	m_TextureCacheMap.Remove(sgaResourceIndex.Value);
}

SGString SGImagePack::getFileName() const {
	return Path::FileName(getPath());
}

bool SGImagePack::hasImgIndex(const SGString& imgName) const {
	return m_Package->HasElementIndex(imgName);
}

SGString& SGImagePack::getImgName(const int imgIndex) const {
	return m_Package->Get(imgIndex)->GetName();
}

void SGImagePack::applyLinearDodge(Byte* pixelData, int len) const {
	for (int i = 0; i < len; i += 4)
	{
		const Byte max = Math::Max(pixelData[i + 0], Math::Max(pixelData[i + 1], pixelData[i + 2]));
		const Byte sub = 0xff - max;

		/* a */ pixelData[i + 3] = Math::Min(pixelData[i + 3], max);
		/* b */ pixelData[i + 2] += sub;	
		/* g */ pixelData[i + 1] += sub;	
		/* r */ pixelData[i + 0] += sub;	
	}
}

void SGImagePack::unload() {
	if (m_bHasLoadedData) {
		return;
	}

	m_Package->UnloadAllElementData();
	m_bHasLoadedData = false;
}

void SGImagePack::clearCache() {
	m_TextureCacheMap.Values().Extension().ForEach([](SGFrameTexture* tex) {
		//DebugAssertMsg(tex->getReferenceCount() == 1, "레퍼런스 카운트가 1이 아닙니다.");
		CC_SAFE_RELEASE(tex);
	});
	m_TextureCacheMap.Clear();
}


