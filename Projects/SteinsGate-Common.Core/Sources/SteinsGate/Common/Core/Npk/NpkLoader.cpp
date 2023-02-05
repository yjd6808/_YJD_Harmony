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

Vector<NpkElement::Header> NpkLoader::ReadPackageIndex(Stream& stream, int elementCount) {
	
	Vector<NpkElement::Header> vHeaders{ elementCount };

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


		/*
		이거 안됨 수정필요함. 일단 스킵한다.
		Vector<NpkElement::Header> vHeaders{ elementCount, {} };
		vHeaders[i] = {
			NpkElement::Header
			{
				Offset,
				0,
				i,
				Length,
				Name
			},
		};
		*/
	}

	// item1: NpkElement::Header
	// item2: int
	for (int i = 0; i < elementCount; ++i) {
		vHeaders[i].NextOffset = i < elementCount - 1 ? vHeaders[i + 1].Offset : stream.GetLength();
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

NpkPackagePtr NpkLoader::Load(const JCore::String& npkPath, int indexOnly, bool headerOnly) {
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
		spPackage->m_ElementNameToIndex.Insert(vHeaders[i].Name, vHeaders[i].IndexInPackage);
	}

	if (headerOnly) {
		spPackage->m_ElementHeaders = Move(vHeaders);
		return spPackage;
	}


	for (int i = 0; i < vHeaders.Size(); ++i) {
		NpkElement::Header& header = vHeaders[i];
		NpkElementPtr spElement = ReadElement(spStream.GetRef(), header, header.NextOffset, indexOnly);
		DebugAssertMsg(spElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
		spElement->m_spParent = spPackage;
		spPackage->Add(header.IndexInPackage, spElement);
	}

	spPackage->m_ElementHeaders = Move(vHeaders);
	return spPackage;
}

NpkPackagePtr NpkLoader::LoadPerfectly(const String& npkPath) {
	return Load(npkPath, false, false);
}

NpkPackagePtr NpkLoader::LoadIndexOnly(const String& npkPath) {
	return Load(npkPath, true, false);
}

NpkPackagePtr NpkLoader::LoadHeaderOnly(const JCore::String& npkPath) {
	return Load(npkPath, true, true);
}

JCore::Vector<JCore::String> NpkLoader::LoadAllImagePackPaths() {
	return Directory::Files(ImagePackPath, true);
}

JCore::Vector<JCore::String> NpkLoader::LoadAllSoundPackPaths() {
	return Directory::Files(SoundPackPath, true);
}

