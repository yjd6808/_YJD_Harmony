/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 1:42:47 AM
 * =====================
 * 
 */

#include "sg/Sga/SgaLoader.h"
#include "sg/Sga/SgaSprite.h"
#include "sg/Sga/SgaSound.h"
#include "sg/Sga/SgaElementInitializer.h"

#include "jc/Primitives/String.h"

#include "jc/Tuple.h"
#include "jc/IO/FileStream.h"
#include "jc/IO/Directory.h"
#include "jc/IO/Path.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
Vector<SgaElement::Header> SgaLoader::ReadPackageIndex(Stream& _stream, int _elementCount)
{
	Vector<SgaElement::Header> headers{ _elementCount };

	for (int i = 0; i < _elementCount; ++i)
	{
		int offset = _stream.ReadInt32();
		int length = _stream.ReadInt32();
		String elementPath = ReadElementPath(_stream);
		String elementName = Path::FileName(elementPath);

		headers.PushBack({
			offset,
			0,
			i,
			length,
			elementName
		});
	}

	// item1: SgaElement::Header
	// item2: int
	for (int i = 0; i < _elementCount; ++i)
	{
		headers[i].nextOffset_ = i < _elementCount - 1 ? headers[i + 1].offset_ : _stream.GetLength();
	}

	return headers;
}

//////////////////////////////////////////////////////////////////////////////////////////
String SgaLoader::ReadElementPath(Stream& _stream)
{
	String elementPath{ SGA_IMG_PATH_LEN };
	char* pElementPathBuffer = elementPath.Source();
	int length = 0;

	for (int i = 0; i < SGA_IMG_PATH_LEN && !_stream.IsEnd(); ++i)
	{
		pElementPathBuffer[i] = _stream.ReadByte();

		if (pElementPathBuffer[i] == NULL)
		{
			length = i;
			break;
		}
	}

	elementPath.SetLength(length);
	int nextPosition = SGA_IMG_PATH_LEN - length - 1;
	_stream.Seek(nextPosition, Stream::Origin::eCurrent);
	return elementPath;
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaElementPtr SgaLoader::ReadElement(Stream& _stream, SgaElement::Header& _header, int _nextOffset, bool _elementOnly)
{
	_stream.Seek(_header.offset_);

	SgaElementPtr pElement;
	String elementFlag = _stream.ReadString();

	if (elementFlag == SgaLoader::ImgFlag)
	{
		_s32 indexLength = static_cast<_s32>(_stream.ReadInt64());
		_s32 version = _stream.ReadInt32();
		_s32 spriteCount = _stream.ReadInt32();

		pElement = SgaImage::Create(_header, spriteCount);
		pElement->indexLength_ = indexLength;
		pElement->version_ = version;
		pElement->indexOffset_ = _stream.GetOffset();

		if (_elementOnly)
		{
			return pElement;
		}

		SgaElementInitializer::InitializeElement(pElement, _stream, true);
		return pElement;
	}

	if (elementFlag == SgaLoader::SpriteFlag)
	{
		_s32 indexLength = _stream.ReadInt32();
		_stream.Seek(2, Stream::eCurrent);
		_s32 version = _stream.ReadInt32();
		_s32 spriteCount = _stream.ReadInt32();

		pElement = SgaImage::Create(_header, spriteCount);
		pElement->indexLength_ = indexLength;
		pElement->version_ = version;
		pElement->indexOffset_ = _stream.GetOffset();

		if (_elementOnly)
		{
			return pElement;
		}

		SgaElementInitializer::InitializeElement(pElement, _stream, true);
		return pElement;
	}

	if (_header.name_.EndWith(".wav") ||
		_header.name_.EndWith(".ogg"))
	{
		_stream.Seek(_header.offset_);
		pElement = SgaSound::Create(_header);
		pElement->version_ = 0;
		pElement->indexLength_ = _nextOffset - _stream.GetOffset(); // 인덱스 길이가 곧 음악 데이터 길이이다.
		pElement->indexOffset_ = _stream.GetOffset();

		if (_elementOnly)
		{
			return pElement;
		}

		SgaElementInitializer::InitializeElement(pElement, _stream, true);
		return pElement;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaPackagePtr SgaLoader::Load(const String& _sgaPath, int _indexOnly, bool _headerOnly)
{
	StreamPtr pStream = MakeShared<FileStream>(_sgaPath, FileAccess::eRead, FileMode::eOpen);
	String flag = pStream->ReadString();

	if (flag != SgaFlag)
	{
		return nullptr;
	}

	int elementCount = pStream->ReadInt32();
	SgaPackagePtr pPackage = SgaPackage::Create(pStream, _sgaPath, elementCount);

	if (elementCount == 0)
	{
		return pPackage;
	}

	auto headers = ReadPackageIndex(pStream.GetRef(), elementCount);

	for (int i = 0; i < headers.Size(); ++i)
	{
		pPackage->elementNameToIndex_.Insert(headers[i].name_, headers[i].indexInPackage_);
	}

	if (_headerOnly)
	{
		pPackage->elementHeaders_ = Move(headers);
		return pPackage;
	}

	for (int i = 0; i < headers.Size(); ++i)
	{
		SgaElement::Header& header = headers[i];
		SgaElementPtr pElement = ReadElement(pStream.GetRef(), header, header.nextOffset_, _indexOnly);
		jc_assert_msg(pElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
		pElement->pParent_ = pPackage;
		pPackage->Add(header.indexInPackage_, pElement);
	}

	pPackage->elementHeaders_ = Move(headers);
	return pPackage;
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaPackagePtr SgaLoader::LoadPerfectly(const String& _sgaPath)
{
	return Load(_sgaPath, false, false);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaPackagePtr SgaLoader::LoadIndexOnly(const String& _sgaPath)
{
	return Load(_sgaPath, true, false);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaPackagePtr SgaLoader::LoadHeaderOnly(const String& _sgaPath)
{
	return Load(_sgaPath, true, true);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> SgaLoader::LoadAllImagePackPaths(const String& _path)
{
	return Directory::Files(_path.Source(), true);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> SgaLoader::LoadAllSoundPackPaths(const String& _path)
{
	return Directory::Files(_path.Source(), true);
}
