/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:41:15 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/SgaPackage.h>

namespace JCore
{
template <typename...>
struct Tuple;
class FileStream;
}

class SgaLoader
{
public:
	inline static const char* SgaFlag = "For SteinsGate Sga";
	inline static const char* ImgFlag = "For SteinsGate Img"; // 버전퍼
	inline static const char* SpriteFlag = "For SteinsGate Sprite"; // 버전세
	static constexpr int SGA_IMG_PATH_LEN = 40;

	static SgaPackagePtr
	Load(const SGString& _sgaPath, int _indexOnly, bool _headerOnly);

	// 로딩 레벨 3: 전부 로딩
	static SgaPackagePtr
	LoadPerfectly(const SGString& _sgaPath);

	// 로딩 레벨 2: 이미지 인덱스까지 로딩
	static SgaPackagePtr
	LoadIndexOnly(const SGString& _sgaPath);

	// 로딩 레벨 1: 패키지 인덱스까지 로딩
	static SgaPackagePtr
	LoadHeaderOnly(const SGString& _sgaPath);

	static SGVector<SGString>
	LoadAllImagePackPaths(const SGString& _path);

	static SGVector<SGString>
	LoadAllSoundPackPaths(const SGString& _path);

	static SGVector<SgaElement::Header>
	ReadPackageIndex(JCore::Stream& _stream, int _elementCount); // 패키지 목차읽기

	static SGString
	ReadElementPath(JCore::Stream& _stream);

	static SgaElementPtr
	ReadElement(JCore::Stream& _stream, SgaElement::Header& _header, int _nextOffset, bool _elementOnly);
};
