/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:06 AM
 * =====================
 *
 */


#pragma once


#include "Tutturu.h"

#include <SteinsGate/Client/SGFrameTexture.h>

#include <SteinsGate/Common/SgaPackage.h>
#include <SteinsGate/Common/SgaImage.h>

class SGImagePack
{
public:
	SGImagePack(const SgaPackagePtr& sgaPackage, int packIndex);
	~SGImagePack();

	int getImgCount() { return m_Package->Count(); }
	int getSpriteCount(int imgIndex);

	SGFrameTexture* createFrameTexture(int imgIndex, int frameIndex);

	void releaseFrameTexture(int imgIndex, int frameIndex);
	void releaseFrameTexture(const SgaResourceIndex& sgaResourceIndex);

	const SGString& getPath() { return m_Package->GetPath(); }
	SGString getFileName();
	bool hasImgIndex(const SGString& imgName);
	int getImgIndex(const SGString& imgName) { return m_Package->GetElementIndex(imgName); }
	SGString& getImgName(const int imgIndex);

	int getPackIndex() { return m_iIndex; }

	void unload();
	void clearCache();
	
private:
	int m_iIndex;
	bool m_bHasLoadedData;	// 하나라도 로딩한 데이터가 있는지
	SgaPackagePtr m_Package;
	SGHashMap<Int64, SGFrameTexture*> m_TextureCacheMap;
};