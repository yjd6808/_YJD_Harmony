/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:41:15 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Core/Npk/NpkPackage.h>

namespace JCore {
	template <typename...>
	struct Tuple;
	class FileStream;
}

class NpkLoader
{
public:
	inline static const char* NpkFlag = "NeoplePack_Bill";
    inline static const char* ImgFlag = "Neople Img File";
	inline static const char* SpriteFlag = "Neople Image File";

	// NPK 바이너리보면 55버전은 키 필요없음; 60버전부터 필요
	inline static const Byte ElementPathKey[] = {
		0x70, 0x75, 0x63, 0x68, 0x69, 0x6B, 0x6F, 0x6E, 0x40, 0x6E,
		0x65, 0x6F, 0x70, 0x6C, 0x65, 0x20, 0x64, 0x75, 0x6E, 0x67,
		0x65, 0x6F, 0x6E, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x66, 0x69,
		0x67, 0x68, 0x74, 0x65, 0x72, 0x20, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E,
		0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46,
		0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44, 0x4E, 0x46, 0x44,
		0x4E, 0x46, 0x44, 0x4E, 0x46, 0x0
	};

	static NpkPackagePtr
	Load(const JCore::String& npkPath, int indexOnly, bool headerOnly);

	// 로딩 레벨 3: 전부 로딩
	static NpkPackagePtr
	LoadPerfectly(const JCore::String& npkPath);

	// 로딩 레벨 2: 이미지 인덱스까지 로딩
	static NpkPackagePtr
	LoadIndexOnly(const JCore::String& npkPath);

	// 로딩 레벨 1: 패키지 인덱스까지 로딩
	static NpkPackagePtr
	LoadHeaderOnly(const JCore::String& npkPath);
	
	static JCore::Vector<JCore::String>
	LoadAllImagePackPaths();

	static JCore::Vector<JCore::String>
	LoadAllSoundPackPaths();

	static JCore::Vector<NpkElement::Header>
	ReadPackageIndex(JCore::Stream& stream, int elementCount);	// 패키지 목차읽기

	static JCore::String
	ReadElementPath(JCore::Stream& stream);

	static NpkElementPtr
	ReadElement(JCore::Stream& stream, NpkElement::Header& header, int nextOffset, bool loadIndexOnly);

};


