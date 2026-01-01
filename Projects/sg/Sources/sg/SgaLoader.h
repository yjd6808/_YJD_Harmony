/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:41:15 AM
 * =====================
 *
 */


#pragma once


#include <sg/SgaPackage.h>

namespace jc
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
	Load(const jc::String& _sgaPath, int _indexOnly, bool _headerOnly);

	// 로딩 레벨 3: 전부 로딩
	static SgaPackagePtr
	LoadPerfectly(const jc::String& _sgaPath);

	// 로딩 레벨 2: 이미지 인덱스까지 로딩
	static SgaPackagePtr
	LoadIndexOnly(const jc::String& _sgaPath);

	// 로딩 레벨 1: 패키지 인덱스까지 로딩
	static SgaPackagePtr
	LoadHeaderOnly(const jc::String& _sgaPath);

	static jc::Vector<jc::String>
	LoadAllImagePackPaths(const jc::String& _path);

	static jc::Vector<jc::String>
	LoadAllSoundPackPaths(const jc::String& _path);

	static jc::Vector<SgaElement::Header>
	ReadPackageIndex(jc::Stream& _stream, int _elementCount); // 패키지 목차읽기

	static jc::String
	ReadElementPath(jc::Stream& _stream);

	static SgaElementPtr
	ReadElement(jc::Stream& _stream, SgaElement::Header& _header, int _nextOffset, bool _elementOnly);
};
