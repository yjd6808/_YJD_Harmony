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
	int cachedCount_{}; // 현재 캐싱된 텍스쳐 갯수
	int maxCachedCount_{}; // 최대로 캐싱되었던 텍스쳐 갯수
};

class ImagePack
{
public:
	ImagePack(const SgaPackagePtr& _pSgaPackage, int _packIndex);
	~ImagePack();

	int GetImgCount() { return pPackage_->Count(); }
	int GetSpriteCount(int _imgIndex) const;

	SgaSpriteAbstractPtr GetSprite(int _imgIndex, int _frameIndex) const;
	SgaSpriteAbstractPtr GetSpriteUnsafe(int _imgIndex, int _frameIndex) const;
	FrameTexture* CreateFrameTexture(int _imgIndex, int _frameIndex, bool _linearDodge = false);

	void ReleaseFrameTexture(int _imgIndex, int _frameIndex);
	void ReleaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex);

	const SGString& GetPath() const { return pPackage_->GetPath(); }
	SGString GetFileName() const;
	bool HasImgIndex(const SGString& _imgName) const;
	int GetImgIndex(const SGString& _imgName) const { return pPackage_->GetElementIndex(_imgName); }
	int GetImgIndex(const char* _imgName) const { return pPackage_->GetElementIndex(_imgName); }
	int GetImgIndexUnsafe(const char* _imgName) const;
	SGString GetImgName(const int _imgIndex) const;
	SGString GetImgNameOrDefault(const int _imgIndex, const SGString& _defaultValue) const;
	void ApplyLinearDodge(Byte* _pPixelData, int _len) const;

	int GetPackIndex() { return index_; }

	void Unload();
	void ClearCache();

private:
	int index_;
	bool hasLoadedData_; // 하나라도 로딩한 데이터가 있는지
	SgaPackagePtr pPackage_;
	SGHashMap<Int64, FrameTexture*> textureCacheMap_;
};
