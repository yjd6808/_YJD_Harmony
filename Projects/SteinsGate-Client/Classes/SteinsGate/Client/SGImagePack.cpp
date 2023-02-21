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

int SGImagePack::getSpriteCount(int imgIndex) {

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndexOnly(imgIndex);

	SgaElementPtr spElement = m_Package->Get(imgIndex);
	SgaImage* pImg = spElement.Get<SgaImage*>();

	if (!pImg->IndexLoaded())
		pImg->LoadIndexOnly();

	return pImg->Count();
}

/*
 * 생성한 텍스쳐는 캐싱해놓자.
 * 같은 몬스터 수십마리를 매번 파일스트림에서 텍스쳐 데이터를 읽고 압축 해제하고 32bit 이미지화 시킬 수는 없자나?
 */
SGFrameTexture* SGImagePack::createFrameTexture(int imgIndex, int frameIndex) {
	SgaResourceIndex index;
	index.Un.SgaIndex = m_iIndex;
	index.Un.ImgIndex = imgIndex;
	index.Un.FrameIndex = frameIndex;

	if (m_TextureCacheMap.Exist(index.Value)) {
		SGFrameTexture* pCached = m_TextureCacheMap[index.Value];
		return pCached;
	}

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndexOnly(imgIndex);

	SgaImage& img = (SgaImage&)m_Package->GetAtRef(imgIndex);

	if (!img.IndexLoaded() && !img.LoadIndexOnly()) {
		return nullptr;
	}

	if (frameIndex < 0 || frameIndex >= img.Count()) {
		DebugAssert(false);
	}

	SgaSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	if (sprite.IsLink()) {
		int iTargetFrameIndex = sprite.GetTargetFrameIndex();
		SGLinkFrameTexture* pLinkTexture = dbg_new SGLinkFrameTexture(frameIndex, iTargetFrameIndex);
		pLinkTexture->autorelease();
		pLinkTexture->retain();
		m_bHasLoadedData = true;
		m_TextureCacheMap.Insert(index.Value, pLinkTexture);
		return pLinkTexture;
	}

	if (sprite.IsDummy()) {
		return nullptr;
	}

	if (!sprite.Loaded())
		sprite.Load();


	SgaDataPtr spData = sprite.Decompress();
	Texture2D* pTexture = dbg_new Texture2D;
	pTexture->initWithData(
		spData.GetPtr(),
		spData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(), { sprite.GetWidthF(), sprite.GetHeightF() }
	);

	auto pSpriteTexture = dbg_new SGSpriteFrameTexture(pTexture, sprite.GetRect(), sprite.GetFrameIndex(), sprite.IsDummy());
	pSpriteTexture->autorelease();
	pSpriteTexture->retain();	// m_TextureCacheMap에서 수명연장시키기 위한 용도
	m_bHasLoadedData = true;
	m_TextureCacheMap.Insert(index.Value, pSpriteTexture);
	return pSpriteTexture;
}

void SGImagePack::releaseFrameTexture(int imgIndex, int frameIndex) {
	SgaResourceIndex index{ m_iIndex, imgIndex, frameIndex };
	DebugAssertMsg(m_TextureCacheMap.Exist(index.Value), "해당하는 프레임 데이터가 없습니다. [1]");
	CC_SAFE_RELEASE(m_TextureCacheMap[index.Value]);
	m_TextureCacheMap.Remove(index.Value);
}

void SGImagePack::releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex) {
	DebugAssertMsg(m_TextureCacheMap.Exist(sgaResourceIndex.Value), "해당하는 프레임 데이터가 없습니다. [2]");
	CC_SAFE_RELEASE(m_TextureCacheMap[sgaResourceIndex.Value]);
	m_TextureCacheMap.Remove(sgaResourceIndex.Value);
}

SGString SGImagePack::getFileName() {
	return Path::FileName(getPath());
}

bool SGImagePack::hasImgIndex(const SGString& imgName) {
	return m_Package->HasElementIndex(imgName);
}

SGString& SGImagePack::getImgName(const int imgIndex) {
	return m_Package->Get(imgIndex)->GetName();
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


