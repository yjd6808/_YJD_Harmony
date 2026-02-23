/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:06 AM
 * =====================
 *
 */


#pragma once


#include <sg/_Sga/SgaSpriteAbstract.h>
#include <sg/_Sga/SgaPackage.h>
#include <sg/_Sga/SgaImage.h>

#include <sgcl/FrameTexture.h>

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

	const jc::String& GetPath() const { return pPackage_->GetPath(); }
	jc::String GetFileName() const;
	bool HasImgIndex(const jc::String& _imgName) const;
	int GetImgIndex(const jc::String& _imgName) const { return pPackage_->GetElementIndex(_imgName); }
	int GetImgIndex(const char* _imgName) const { return pPackage_->GetElementIndex(_imgName); }
	int GetImgIndexUnsafe(const char* _imgName) const;
	jc::String GetImgName(const int _imgIndex) const;
	jc::String GetImgNameOrDefault(const int _imgIndex, const jc::String& _defaultValue) const;
	void ApplyLinearDodge(_u8* _pPixelData, int _len) const;

	int GetPackIndex() { return index_; }

	void Unload();
	void ClearCache();

private:
	int index_;
	bool hasLoadedData_; // 하나라도 로딩한 데이터가 있는지
	SgaPackagePtr pPackage_;
	jc::HashMap<_s64, FrameTexture*> textureCacheMap_;
};
