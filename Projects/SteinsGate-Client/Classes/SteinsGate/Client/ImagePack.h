/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:06 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Type.h>
#include <SteinsGate/Common/SgaSpriteAbstract.h>
#include <SteinsGate/Common/SgaPackage.h>
#include <SteinsGate/Common/SgaImage.h>

#include <SteinsGate/Client/FrameTexture.h>

struct CachedTextureCounter
{
	int CachedCount{};			// 현재 캐싱된 텍스쳐 갯수
	int MaxCachedCount{};		// 최대로 캐싱되었던 텍스쳐 갯수
};

class ImagePack
{
public:
	ImagePack(const SgaPackagePtr& sgaPackage, int packIndex);
	~ImagePack();

	int getImgCount() { return m_Package->Count(); }
	int getSpriteCount(int imgIndex) const;

	SgaSpriteAbstractPtr getSprite(int imgIndex, int frameIndex) const;
	SgaSpriteAbstractPtr getSpriteUnsafe(int imgIndex, int frameIndex) const;
	FrameTexture* createFrameTexture(int imgIndex, int frameIndex, bool linearDodge = false);

	void releaseFrameTexture(int imgIndex, int frameIndex);
	void releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex);

	const SGString& getPath() const { return m_Package->GetPath(); }
	SGString getFileName() const;
	bool hasImgIndex(const SGString& imgName) const;
	int getImgIndex(const SGString& imgName) const { return m_Package->GetElementIndex(imgName); }
	int getImgIndex(const char* imgName) const { return m_Package->GetElementIndex(imgName); }
	int getImgIndexUnsafe(const char* imgName) const;
	SGString getImgName(const int imgIndex) const;
	SGString getImgNameOrDefault(const int imgIndex, const SGString& defaultValue) const;
	void applyLinearDodge(Byte* pixelData, int len) const;

	int getPackIndex() { return m_iIndex; }

	void unload();
	void clearCache();
	
private:
	int m_iIndex;
	bool m_bHasLoadedData;	// 하나라도 로딩한 데이터가 있는지
	SgaPackagePtr m_Package;
	SGHashMap<Int64, FrameTexture*> m_TextureCacheMap;
};
