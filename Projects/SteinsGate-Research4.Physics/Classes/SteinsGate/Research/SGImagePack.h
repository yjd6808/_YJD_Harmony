/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 6:36:06 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/FrameTexture.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Common/Core/Npk/NpkImage.h>

class SGImagePack
{
public:
	SGImagePack(const NpkPackagePtr& npkPackage);
	~SGImagePack();

	int count() { return m_Package->Count(); }
	FrameTexture* createFrameTexture(int imgIndex, int frameIndex);
	void releaseFrameTexture(int imgIndex, int frameIndex);
	const SGString& getPath() { return m_Package->GetPath(); }
	SGString getFileName();
	int getIndex(const SGString& imgName) { return m_Package->GetElementIndex(imgName); }
	bool hasIndex(const SGString& imgName);
private:
	NpkPackagePtr m_Package;
	SGHashMap<Int64, FrameTexture*> m_TextureCacheMap;
};