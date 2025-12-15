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
	int CachedCount{}; // 현재 캐싱된 텍스쳐 갯수
	int MaxCachedCount{}; // 최대로 캐싱되었던 텍스쳐 갯수
};

class ImagePack
{
public:
	ImagePack(const SgaPackagePtr& _pSgaPackage, int _packIndex);
	~ImagePack();

	int getImgCount() { return package_->Count(); }
	int getSpriteCount(int _imgIndex) const;

	SgaSpriteAbstractPtr getSprite(int _imgIndex, int _frameIndex) const;
	SgaSpriteAbstractPtr getSpriteUnsafe(int _imgIndex, int _frameIndex) const;
	FrameTexture* createFrameTexture(int _imgIndex, int _frameIndex, bool _linearDodge = false);

	void releaseFrameTexture(int _imgIndex, int _frameIndex);
	void releaseFrameTexture(const SgaResourceIndex& _sgaResourceIndex);

	const SGString& getPath() const { return package_->GetPath(); }
	SGString getFileName() const;
	bool hasImgIndex(const SGString& _imgName) const;
	int getImgIndex(const SGString& _imgName) const { return package_->GetElementIndex(_imgName); }
	int getImgIndex(const char* _imgName) const { return package_->GetElementIndex(_imgName); }
	int getImgIndexUnsafe(const char* _imgName) const;
	SGString getImgName(const int _imgIndex) const;
	SGString getImgNameOrDefault(const int _imgIndex, const SGString& _defaultValue) const;
	void applyLinearDodge(Byte* _pPixelData, int _len) const;

	int getPackIndex() { return index_; }

	void unload();
	void clearCache();

private:
	int index_;
	bool hasLoadedData_; // 하나라도 로딩한 데이터가 있는지
	SgaPackagePtr package_;
	SGHashMap<Int64, FrameTexture*> textureCacheMap_;
};
