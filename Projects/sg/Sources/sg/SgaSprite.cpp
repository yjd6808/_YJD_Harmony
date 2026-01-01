/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:00:40 AM
 * =====================
 *
 */

#include "Core.h"
#include "SgaSprite.h"

#include <sg/SgaPackage.h>
#include <sg/SgaColorFormat.h>

#include <jc/Stream.h>
#include <zlib.h>

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SgaDataPtr SgaSprite::Decompress()
{
	if (!loaded_)
		return nullptr;

	int decompressedSize = rect_.width_ * rect_.height_ * (m_eColorFormat == SgaColorFormat::eArgb8888 ? 4 : 2);

	SgaDataPtr pDecompressedData = nullptr;
	Byte* pReadData = pData_.GetPtr();

	if (compressMode_ == SgaCompressMode::eZlib)
	{
		Int32UL decompressedSizeTemp;
		pDecompressedData = MakeShared<Byte[]>(decompressedSize);
		Byte* pDecompressedRawPtr = pDecompressedData.GetPtr();
		uncompress(pDecompressedRawPtr, &decompressedSizeTemp, pData_.GetPtr(), dataLength_);
		pReadData = pDecompressedRawPtr;
	}

	// 기본적으로 던파 픽셀 포맷은 다이렉트X 기반이라 32비트 BGRA임
	// Cocos2d-x는 OpenGL 기반이라 픽셀 포맷을 RGBA 순서로 변경해줘야한다.
	// Sga 패키지들중 Bgra 32비트 픽셀 포맷 타입들은 전부 Rgba32비트 픽셀 포맷으로 변경해서 저장해야겠다.
	//  => 작업완료 이제 8888 포맷들은 변환안해도댐
	if (m_eColorFormat == SgaColorFormat::eArgb8888)
	{
		SgaDataPtr& pData = pDecompressedData == nullptr ? pData_ : pDecompressedData;
		return pData;
	}

	int bitSize32 = decompressedSize * 2;
	auto pBits32 = MakeShared<Byte[]>(bitSize32);

	Byte* pRaw32Bytes = pBits32.GetPtr();
	Byte readBytes[2];

	for (int i = 0, j = 0; i < bitSize32; i += 4, j += 2)
	{
		Byte a = 0;
		Byte r = 0;
		Byte g = 0;
		Byte b = 0;

		readBytes[0] = pReadData[j];
		readBytes[1] = pReadData[j + 1];

		// 순서: A R R R R R G G | G G G B B B B B
		switch (m_eColorFormat)
		{
		case SgaColorFormat::eArgb1555:
			a = static_cast<Byte>(readBytes[1] >> 7);
			r = static_cast<Byte>((readBytes[1] >> 2) & 0x1f);
			g = static_cast<Byte>((readBytes[0] >> 5) | ((readBytes[1] & 3) << 3));
			b = static_cast<Byte>(readBytes[0] & 0x1f);
			a = static_cast<Byte>(a * 0xff);
			r = static_cast<Byte>((r << 3) | (r >> 2));
			g = static_cast<Byte>((g << 3) | (g >> 2));
			b = static_cast<Byte>((b << 3) | (b >> 2));
			break;

		// 순서: A A A A R R R R | G G G G B B B B
		case SgaColorFormat::eArgb4444:
			a = static_cast<Byte>(readBytes[1] & 0xf0);
			r = static_cast<Byte>((readBytes[1] & 0xf) << 4);
			g = static_cast<Byte>(readBytes[0] & 0xf0);
			b = static_cast<Byte>((readBytes[0] & 0xf) << 4);
			break;
		default:
			jc_assert(false);
		}

		// 32비트 픽셀 포맷으로 확장할때 한번에 Rgba32 포맷으로 변경하자.
		pRaw32Bytes[i] = r;
		pRaw32Bytes[i + 1] = g;
		pRaw32Bytes[i + 2] = b;
		pRaw32Bytes[i + 3] = a;
	}

	return pBits32;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaSprite::Load()
{
	if (!m_spParent.Exist())
	{
		return false;
	}

	SgaPackage* pPackage = m_spParent->GetParent();
	if (pPackage == nullptr)
	{
		return false;
	}

	pData_ = MakeShared<Byte[]>(dataLength_);
	Stream& stream = pPackage->StreamRef();
	stream.Seek(dataOffset_);
	stream.Read(pData_.GetPtr(), 0, dataLength_);
	loaded_ = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaSprite::Unload()
{
	if (!pData_.Exist())
	{
		return false;
	}

	pData_ = nullptr;
	loaded_ = false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSpritePtr SgaSprite::Create(const SgaImagePtr& _pParent, int _frameIndex, int _format)
{
	return MakeShared<SgaSprite>(_pParent, _frameIndex, _format);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSprite::~SgaSprite()
{
}
