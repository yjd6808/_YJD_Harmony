/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:25 AM
 * =====================
 *
 */


#include "SGImagePack.h"

#include <JCore/FileSystem/Path.h>
#include <SteinsGate/Common/Core/Npk/NpkSpriteAbstract.h>

USING_NS_CC;

using namespace JCore;


SGImagePack::SGImagePack(const NpkPackagePtr& npkPackage)
	: m_Package(npkPackage)
	, m_TextureCacheMap(1'000) {
}

SGImagePack::~SGImagePack() {
	m_TextureCacheMap
	.Values()
	.Extension()
	.ForEach([](FrameTexture* tex) {
		CC_SAFE_RELEASE(tex);
	});
}


FrameTexture* SGImagePack::createFrameTexture(int imgIndex, int frameIndex) {
	Int64 iCacheIndex = (Int64U)imgIndex << 32 | frameIndex;

	if (m_TextureCacheMap.Exist(iCacheIndex)) {
		return m_TextureCacheMap[iCacheIndex];
	}

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndexOnly(imgIndex);

	const char* szPackpath = m_Package->GetPath().Source();		// 디버깅용
	NpkImage& img = (NpkImage&)m_Package->GetAtRef(imgIndex);
	const char* szImgPath = img.GetName().Source();						// 디버깅용

	if (!img.IndexLoaded() && !img.LoadIndexOnly()) {
		return nullptr;
	}

	if (frameIndex < 0 || frameIndex >= img.Count()) {
		DebugAssert(false);
	}

	NpkSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	if (sprite.IsLink()) {
		LinkFrameTexture* pLinkTexture = new LinkFrameTexture(sprite.GetTargetFrameIndex());
		pLinkTexture->autorelease();
		pLinkTexture->retain();
		m_TextureCacheMap.Insert(iCacheIndex, pLinkTexture);
		return pLinkTexture;
	}

	if (sprite.IsDummy()) {
		return nullptr;
	}

	if (!sprite.Loaded())
		sprite.Load();

	auto spData = sprite.Decompress();
	Texture2D* pTexture = new Texture2D;
	pTexture->initWithData(
		spData.GetPtr(),
		spData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(), { sprite.GetWidthF(), sprite.GetHeightF() }
	);

	auto pSpriteTexture = new SpriteFrameTexture(pTexture, sprite.GetRect(), sprite.GetFrameIndex(), sprite.IsDummy());
	pSpriteTexture->autorelease();
	pSpriteTexture->retain();
	m_TextureCacheMap.Insert(iCacheIndex, pSpriteTexture);
	return pSpriteTexture;
}

void SGImagePack::releaseFrameTexture(int imgIndex, int frameIndex) {
	Int64 iCacheIndex = imgIndex << 32 | frameIndex;
	DebugAssertMessage(m_TextureCacheMap.Exist(iCacheIndex), "해당하는 프레임 데이터가 없습니다.");
	CC_SAFE_RELEASE(m_TextureCacheMap[iCacheIndex]);
}

SGString SGImagePack::getFileName() {
	return Path::FileName(getPath());
}

bool SGImagePack::hasIndex(const SGString& imgName) {
	return m_Package->HasElementIndex(imgName);
}


