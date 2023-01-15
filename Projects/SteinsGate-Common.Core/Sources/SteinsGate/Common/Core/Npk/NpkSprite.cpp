/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:00:40 AM
 * =====================
 *
 */

#include <SteinsGate/Common/Core/Npk/NpkSprite.h>
#include <SteinsGate/Common/Core/Npk/NpkColorFormat.h>

#include <JCore/Stream.h>
#include <zlib.h>

#include "NpkPackage.h"

using namespace JCore;

// 버전 세칸도 기준 구현
NpkDataPtr NpkSprite::Decompress() {

    if (!m_bLoaded) return nullptr;

	int iDecompressedSize = m_Rect.Width * m_Rect.Height * (m_eColorFormat == NpkColorFormat::eArgb8888 ? 4 : 2);
    auto spDecompressed = MakeShared<Byte[]>(iDecompressedSize);
    Byte* pDecompressedRawPtr = spDecompressed.GetPtr();

	if (m_eCompressMode == NpkCompressMode::eZlib) {
        Int32UL _;
		uncompress(pDecompressedRawPtr, &_, m_spData.GetPtr(), m_iDataLength);
	}

    if (m_eColorFormat == NpkColorFormat::eArgb8888)
        return spDecompressed;

	int i32BitSize = iDecompressedSize * 2;
    auto sp32Bits = MakeShared<Byte[]>(i32BitSize);

    Byte* pRaw32Bites = sp32Bits.GetPtr();
    Byte ReadBytes[2];
   

	for (int i = 0, j = 0; i < i32BitSize; i += 4, j += 2) {

        Byte a = 0;
        Byte r = 0;
        Byte g = 0;
        Byte b = 0;

        ReadBytes[0] = pDecompressedRawPtr[j];
        ReadBytes[1] = pDecompressedRawPtr[j + 1];

        switch (m_eColorFormat) {
        case NpkColorFormat::eArgb1555:
            a = (Byte)(ReadBytes[1] >> 7);
            r = (Byte)((ReadBytes[1] >> 2) & 0x1f);
            g = (Byte)((ReadBytes[0] >> 5) | ((ReadBytes[1] & 3) << 3));
            b = (Byte)(ReadBytes[0] & 0x1f);
            a = (Byte)(a * 0xff);
            r = (Byte)((r << 3) | (r >> 2));
            g = (Byte)((g << 3) | (g >> 2));
            b = (Byte)((b << 3) | (b >> 2));
            break;
        case NpkColorFormat::eArgb4444:
            a = (Byte)(ReadBytes[1] & 0xf0);
            r = (Byte)((ReadBytes[1] & 0xf) << 4);
            g = (Byte)(ReadBytes[0] & 0xf0);
            b = (Byte)((ReadBytes[0] & 0xf) << 4);
            break;
        default:
            DebugAssert(false);
        }

        pRaw32Bites[i] = r;
        pRaw32Bites[i + 1] = g;
        pRaw32Bites[i + 2] = b;
        pRaw32Bites[i + 3] = a;
	}

	return sp32Bits;
}

bool NpkSprite::Load() {

    if (!m_spParent.Exist()) {
        return false;
    }

    NpkPackage* pPackage = m_spParent->GetParent();
    if (pPackage == nullptr) {
        return false;
    }

    m_spData = MakeShared<Byte[]>(m_iDataLength);
    Stream& stream = pPackage->StreamRef();
    stream.Seek(m_iDataOffset);
    stream.Read(m_spData.GetPtr(), 0, m_iDataLength);
    m_bLoaded = true;
}

void NpkSprite::Unload() {
    m_spData = nullptr;
    m_bLoaded = false;
}

NpkSpritePtr NpkSprite::Create(const NpkImagePtr& parent, int frameIndex, int format) {
	 return MakeShared<NpkSprite>(parent, frameIndex, format);
 }

NpkSprite::~NpkSprite() {
	 
 }
