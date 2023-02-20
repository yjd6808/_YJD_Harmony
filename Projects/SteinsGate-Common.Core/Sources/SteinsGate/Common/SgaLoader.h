/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:41:15 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/SgaPackage.h>

namespace JCore {
	template <typename...>
	struct Tuple;
	class FileStream;
}

class SgaLoader
{
public:
	inline static const char* SgaFlag = "For SteinsGate Sga";
    inline static const char* ImgFlag = "For SteinsGate Img";			// 버전퍼
	inline static const char* SpriteFlag = "For SteinsGate Sprite";		// 버전세
	inline static constexpr int SgaImgPathLen = 40;
	
	static SgaPackagePtr
	Load(const SGString& sgaPath, int indexOnly, bool headerOnly);

	// 로딩 레벨 3: 전부 로딩
	static SgaPackagePtr
	LoadPerfectly(const SGString& sgaPath);

	// 로딩 레벨 2: 이미지 인덱스까지 로딩
	static SgaPackagePtr
	LoadIndexOnly(const SGString& sgaPath);

	// 로딩 레벨 1: 패키지 인덱스까지 로딩
	static SgaPackagePtr
	LoadHeaderOnly(const SGString& sgaPath);
	
	static SGVector<SGString>
	LoadAllImagePackPaths(const SGString& path);

	static SGVector<SGString>
	LoadAllSoundPackPaths(const SGString& path);

	static SGVector<SgaElement::Header>
	ReadPackageIndex(JCore::Stream& stream, int elementCount);	// 패키지 목차읽기

	static SGString
	ReadElementPath(JCore::Stream& stream);

	static SgaElementPtr
	ReadElement(JCore::Stream& stream, SgaElement::Header& header, int nextOffset, bool loadIndexOnly);

};


