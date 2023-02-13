/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:25 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGImagePack.h"

#include <JCore/FileSystem/Path.h>
#include <SteinsGate/Common/Core/Npk/NpkSpriteAbstract.h>
#include <SteinsGate/Client/SGGlobal.h>

USING_NS_CC;
USING_NS_JC;


SGImagePack::SGImagePack(const NpkPackagePtr& npkPackage, int packIndex)
	: m_iIndex(packIndex)
	, m_Package(npkPackage)
	, m_TextureCacheMap(1'000) {
}

SGImagePack::~SGImagePack() {
	m_TextureCacheMap
	.Values()
	.Extension()
	.ForEach([](SGFrameTexture* tex) {
		CC_SAFE_RELEASE(tex);
	});
}

int SGImagePack::getSpriteCount(int imgIndex) {
	NpkElementPtr spElement = m_Package->Get(imgIndex);
	NpkImage* pImg = spElement.Get<NpkImage*>();

	if (!pImg->IndexLoaded())
		pImg->LoadIndexOnly();

	return pImg->Count();
}

/*
 * 생성한 텍스쳐는 캐싱해놓자.
 * 같은 몬스터 수십마리를 매번 파일스트림에서 텍스쳐 데이터를 읽고 압축 해제하고 32bit 이미지화 시킬 수는 없자나?
 */
SGFrameTexture* SGImagePack::createFrameTexture(int imgIndex, int frameIndex) {
	Int64 iCacheIndex = (Int64U)imgIndex << 32 | frameIndex;

	if (m_TextureCacheMap.Exist(iCacheIndex)) {
		return m_TextureCacheMap[iCacheIndex];
	}

	if (!m_Package->IsElementLoaded(imgIndex))
		m_Package->LoadElementIndexOnly(imgIndex);

	const char* szPackpath = m_Package->GetPath().Source();		// 디버깅용
	NpkImage& img = (NpkImage&)m_Package->GetAtRef(imgIndex);
	const char* szImgPath = img.GetName().Source();				// 디버깅용

	if (!img.IndexLoaded() && !img.LoadIndexOnly()) {
		return nullptr;
	}

	if (frameIndex < 0 || frameIndex >= img.Count()) {
		DebugAssert(false);
	}

	NpkSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	if (sprite.IsLink()) {
		int iTargetFrameIndex = sprite.GetTargetFrameIndex();
		SGLinkFrameTexture* pLinkTexture = new SGLinkFrameTexture(frameIndex, iTargetFrameIndex);
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

	auto pSpriteTexture = new SGSpriteFrameTexture(pTexture, sprite.GetRect(), sprite.GetFrameIndex(), sprite.IsDummy());
	pSpriteTexture->autorelease();
	pSpriteTexture->retain();	// m_TextureCacheMap에서 수명연장시키기 위한 용도
	m_TextureCacheMap.Insert(iCacheIndex, pSpriteTexture);
	return pSpriteTexture;
}

void SGImagePack::releaseFrameTexture(int imgIndex, int frameIndex) {
	Int64 iCacheIndex = imgIndex << 32 | frameIndex;
	DebugAssertMsg(m_TextureCacheMap.Exist(iCacheIndex), "해당하는 프레임 데이터가 없습니다.");
	CC_SAFE_RELEASE(m_TextureCacheMap[iCacheIndex]);
	m_TextureCacheMap.Remove(iCacheIndex);
}

SGString SGImagePack::getFileName() {
	return Path::FileName(getPath());
}

bool SGImagePack::hasImgIndex(const SGString& imgName) {
	return m_Package->HasElementIndex(imgName);
}


