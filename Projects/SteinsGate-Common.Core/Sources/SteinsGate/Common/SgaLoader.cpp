/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:42:47 AM
 * =====================
 * 
 */

#include "Core.h"
#include <SteinsGate/Common/SgaLoader.h>
#include <SteinsGate/Common/SgaSprite.h>
#include <SteinsGate/Common/SgaSound.h>
#include <SteinsGate/Common/SgaElementInitializer.h>

#include <JCore/Primitives/String.h>

#include <JCore/Tuple.h>
#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>



USING_NS_JC;

Vector<SgaElement::Header> SgaLoader::ReadPackageIndex(Stream& stream, int elementCount) {
	
	Vector<SgaElement::Header> vHeaders{ elementCount };

	for (int i = 0; i < elementCount; ++i) {
		int Offset = stream.ReadInt32();
		int Length = stream.ReadInt32();
		String Path = ReadElementPath(stream);
		String Name = Path::FileName(Path);

		vHeaders.PushBack({
			Offset,
			0,
			i,
			Length,
			Name
		});


	}

	// item1: SgaElement::Header
	// item2: int
	for (int i = 0; i < elementCount; ++i) {
		vHeaders[i].NextOffset = i < elementCount - 1 ? vHeaders[i + 1].Offset : stream.GetLength();
	}

	return vHeaders;
}


String SgaLoader::ReadElementPath(Stream& stream) {

	String szElementPath{ SgaImgPathLen };
	char* szElementPathSource = szElementPath.Source();
	int iLen = 0;

	for (int i = 0; i < SgaImgPathLen && !stream.IsEnd(); ++i) {

		szElementPathSource[i] = stream.ReadByte();

		if (szElementPathSource[i] == NULL) {
			iLen = i;
			break;
		}

	}

	szElementPath.SetLength(iLen);
	int iNextPosition = SgaImgPathLen - iLen - 1;
	stream.Seek(iNextPosition, Stream::Origin::eCurrent);
	return szElementPath;
}


SgaElementPtr SgaLoader::ReadElement(Stream& stream, SgaElement::Header& header, int nextOffset, bool loadIndexOnly) {
	stream.Seek(header.Offset);

	SgaElementPtr spElement;
	String szElementFlag = stream.ReadString();

	if (szElementFlag == SgaLoader::ImgFlag) {
		Int32 iIndexLength = (Int32)stream.ReadInt64();
		Int32 iVersion = stream.ReadInt32();
		Int32 iSpriteCount = stream.ReadInt32();

		spElement = SgaImage::Create(header, iSpriteCount);
		spElement->m_iIndexLength = iIndexLength;
		spElement->m_iVersion = iVersion;
		spElement->m_iIndexOffset = stream.GetOffset();
		if (loadIndexOnly) return spElement;
		SgaElementInitializer::InitializeElement(spElement, stream, loadIndexOnly);
		return spElement;
	}

	if (szElementFlag == SgaLoader::SpriteFlag) {

		Int32 iIndexLength = stream.ReadInt32(); stream.Seek(2, Stream::eCurrent);
		Int32 iVersion = stream.ReadInt32();
		Int32 iSpriteCount = stream.ReadInt32();

		spElement = SgaImage::Create(header, iSpriteCount);
		spElement->m_iIndexLength = iIndexLength;
		spElement->m_iVersion = iVersion;
		spElement->m_iIndexOffset = stream.GetOffset();

		if (loadIndexOnly) return spElement;
		SgaElementInitializer::InitializeElement(spElement, stream, loadIndexOnly);
		return spElement;
	}

	if (header.Name.EndWith(".wav") ||
		header.Name.EndWith(".ogg")) {
		stream.Seek(header.Offset);
		spElement = SgaSound::Create(header);
		spElement->m_iVersion = 0;
		spElement->m_iIndexLength = nextOffset - stream.GetOffset();	// 인덱스 길이가 곧 음악 데이터 길이이다.
		spElement->m_iIndexOffset = stream.GetOffset();
		if (loadIndexOnly) return spElement;
		SgaElementInitializer::InitializeElement(spElement, stream, loadIndexOnly);
		return spElement;
	}

	return nullptr;
}

SgaPackagePtr SgaLoader::Load(const String& sgaPath, int indexOnly, bool headerOnly) {
	StreamPtr spStream = MakeShared<FileStream>(sgaPath, FileAccess::eRead, FileMode::eOpen);
	String szFlag = spStream->ReadString();

	if (szFlag != SgaFlag) {
		return nullptr;
	}

	int iElementCount = spStream->ReadInt32();
	SgaPackagePtr spPackage = SgaPackage::Create(spStream, sgaPath, iElementCount);

	if (iElementCount == 0)
		return spPackage;

	auto vHeaders = ReadPackageIndex(spStream.GetRef(), iElementCount);

	for (int i = 0; i < vHeaders.Size(); ++i) {
		spPackage->m_ElementNameToIndex.Insert(vHeaders[i].Name, vHeaders[i].IndexInPackage);
	}

	if (headerOnly) {
		spPackage->m_ElementHeaders = Move(vHeaders);
		return spPackage;
	}


	for (int i = 0; i < vHeaders.Size(); ++i) {
		SgaElement::Header& header = vHeaders[i];
		SgaElementPtr spElement = ReadElement(spStream.GetRef(), header, header.NextOffset, indexOnly);
		DebugAssertMsg(spElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
		spElement->m_spParent = spPackage;
		spPackage->Add(header.IndexInPackage, spElement);
	}

	spPackage->m_ElementHeaders = Move(vHeaders);
	return spPackage;
}

SgaPackagePtr SgaLoader::LoadPerfectly(const String& sgaPath) {
	return Load(sgaPath, false, false);
}

SgaPackagePtr SgaLoader::LoadIndexOnly(const String& sgaPath) {
	return Load(sgaPath, true, false);
}

SgaPackagePtr SgaLoader::LoadHeaderOnly(const String& sgaPath) {
	return Load(sgaPath, true, true);
}

SGVector<String> SgaLoader::LoadAllImagePackPaths(const String& path) {
	return Directory::Files(path.Source(), true);
}

SGVector<String> SgaLoader::LoadAllSoundPackPaths(const String& path) {
	return Directory::Files(path.Source(), true);
}

