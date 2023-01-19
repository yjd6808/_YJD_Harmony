/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:42:47 AM
 * =====================
 * 
 */

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Common/Core/Npk/NpkSprite.h>
#include <SteinsGate/Common/Core/Npk/NpkSound.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>

#include <JCore/Primitives/String.h>

#include <JCore/Tuple.h>
#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>



using namespace JCore;

Vector<NpkLoader::NpkElementHeader> NpkLoader::ReadPackageIndex(Stream& stream, int elementCount) {
	
	Vector<NpkElementHeader> vHeaders{ elementCount, {} };

	for (int i = 0; i < elementCount; ++i) {
		int Offset = stream.ReadInt32();
		int Length = stream.ReadInt32();
		String Path = ReadElementPath(stream);
		String Name = Path::FileName(Path);

		vHeaders[i] = {
			NpkElement::Header
			{
				Offset,
				Length,
				Path,
				Name
			},
			0
		};
	}

	// item1: NpkElement::Header
	// item2: int
	for (int i = 0; i < elementCount; ++i) {
		vHeaders[i].item2 = i < elementCount - 1 ? vHeaders[i + 1].item1.Offset : stream.GetLength();
	}

	return vHeaders;
}


String NpkLoader::ReadElementPath(Stream& stream) {

	String szElementPath{ sizeof ElementPathKey };
	char* szElementPathSource = szElementPath.Source();
	int iLen = 0;

	for (int i = 0; i < 256 && !stream.IsEnd(); ++i) {

		szElementPathSource[i] = stream.ReadByte() ^ ElementPathKey[i];

		if (szElementPathSource[i] == NULL) {
			iLen = i;
			break;
		}
	}

	szElementPath.SetLength(iLen);
	int iNextPosition = sizeof ElementPathKey - iLen - 1;
	stream.Seek(iNextPosition, Stream::Origin::eCurrent);
	return szElementPath;
}


NpkElementPtr NpkLoader::ReadElement(Stream& stream, NpkElement::Header& header, int nextOffset, bool loadIndexOnly) {
	stream.Seek(header.Offset);

	NpkElementPtr spElement;
	String szElementFlag = stream.ReadString();

	if (szElementFlag == NpkLoader::ImgFlag) {
		spElement = NpkImage::Create(header);
		spElement->m_iIndexLength = (Int32)stream.ReadInt64();
		spElement->m_iVersion = stream.ReadInt32();
		spElement.Get<NpkImage*>()->m_iWaitForLoadingDataCount = stream.ReadInt32();
		spElement->m_iIndexOffset = stream.GetOffset();
		if (loadIndexOnly) return spElement;
		NpkElementInitializer::InitializeElement(spElement, stream, loadIndexOnly);
		return spElement;
	}

	if (szElementFlag == NpkLoader::SpriteFlag) {
		spElement = NpkImage::Create(header);
		spElement->m_iVersion = 1;
		spElement->m_iIndexOffset = stream.GetOffset();
		if (loadIndexOnly) return spElement;
		NpkElementInitializer::InitializeElement(spElement, stream, loadIndexOnly);
		return spElement;
	}

	if (header.Name.EndWith(".wav") ||
		header.Name.EndWith(".ogg")) {
		stream.Seek(header.Offset);
		spElement = NpkSound::Create(header);
		spElement->m_iVersion = 0;
		spElement->m_iIndexLength = nextOffset - stream.GetOffset();	// 인덱스 길이가 곧 음악 데이터 길이이다.
		spElement->m_iIndexOffset = stream.GetOffset();
		if (loadIndexOnly) return spElement;
		NpkElementInitializer::InitializeElement(spElement, stream, loadIndexOnly);
		return spElement;
	}

	return nullptr;
}

NpkPackagePtr NpkLoader::Load(const JCore::String& npkPath, int indexOnly) {
	StreamPtr spStream = MakeShared<FileStream>(npkPath, FileAccess::eRead, FileMode::eOpen);
	String szFlag = spStream->ReadString();

	if (szFlag != NpkFlag) {
		return nullptr;
	}

	int iElementCount = spStream->ReadInt32();
	NpkPackagePtr spPackage = NpkPackage::Create(spStream, npkPath, iElementCount);

	if (iElementCount == 0)
		return spPackage;

	auto vHeaders = ReadPackageIndex(spStream.GetRef(), iElementCount);

	for (int i = 0; i < vHeaders.Size(); ++i) {
		NpkElementHeader& header = vHeaders[i];
		NpkElementPtr spElement = ReadElement(spStream.GetRef(), header.item1, header.item2, indexOnly);
		DebugAssertMessage(spElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
		spElement->m_spParent = spPackage;
		spPackage->Add(spElement);
	}

	spPackage->m_bLoaded = true;
	return spPackage;
}

NpkPackagePtr NpkLoader::LoadPerfectly(const String& npkPath) {
	return Load(npkPath, false);
}

NpkPackagePtr NpkLoader::LoadIndexOnly(const String& npkPath) {
	return Load(npkPath, true);
}

JCore::Vector<JCore::String> NpkLoader::LoadAllImagePackPaths() {
	return Directory::Files(ImagePackPath, true);
}

JCore::Vector<JCore::String> NpkLoader::LoadAllSoundPackPaths() {
	return Directory::Files(SoundPackPath, true);
}

