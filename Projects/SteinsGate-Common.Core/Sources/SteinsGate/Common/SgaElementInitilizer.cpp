/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:08:22 PM
 * =====================
 */

#include "Core.h"
#include <SteinsGate/Common/SgaElementInitializer.h>
#include <SteinsGate/Common/SgaLinkSprite.h>
#include <SteinsGate/Common/SgaSprite.h>
#include <SteinsGate/Common/SgaSound.h>
#include <SteinsGate/Common/SgaColorFormat.h>

USING_NS_JC;

void SgaElementInitializerImpl<-1>::Initialize(const SgaElementPtr& element, Stream& stream, bool indexOnly)
{
	DebugAssertMsg(false, "이상한 버전이군요");
}

void SgaElementInitializerImpl<0>::Initialize(const SgaElementPtr& element, Stream& stream, bool indexOnly)
{
	
	SgaSound& sound = *element.Get<SgaSound*>();
	stream.Seek(sound.m_iIndexOffset);
	sound.m_bIndexLoaded = true;
	if (indexOnly) return;
	auto pData = MakeShared<Byte[]>(sound.m_iIndexLength);
	stream.Read(pData.GetPtr(), 0, pData.Length());
	sound.m_spData = pData;
}

void SgaElementInitializerImpl<1>::Initialize(const SgaElementPtr& element, Stream& stream, bool indexOnly)
{
	SgaImage& image = *element.Get<SgaImage*>();
	stream.Seek(image.m_iIndexOffset);
	int iSpriteDataOffset = stream.GetOffset() + element->m_iIndexLength;
	int iWaitForLoadingCount = image.WaitForLoadingDataCount();

	DebugAssertMsg(iWaitForLoadingCount == image.Count(), "로딩해야할 데이터 수와 미리 생성된 NULL 벡터의 용량이랑 다릅니다. (%d:%d)", iWaitForLoadingCount, image.Count());

	for (int i = 0; i < iWaitForLoadingCount; ++i) {
		auto eFormat = (SgaColorFormat)stream.ReadInt32();
		if (eFormat == SgaColorFormat::eLink) {
			auto spLinkSprite = SgaLinkSprite::Create(element, i, eFormat);
			spLinkSprite->m_iTargetFrameIndex = stream.ReadInt32();
			image.Set(i, spLinkSprite);
			continue;
		}

		auto spSprite = SgaSprite::Create(element, i, eFormat);
		auto pSprite = spSprite.GetPtr();

		pSprite->m_eCompressMode = stream.ReadInt32();
		pSprite->m_Rect.Width = stream.ReadInt32();
		pSprite->m_Rect.Height = stream.ReadInt32();
		pSprite->m_iDataLength = stream.ReadInt32();
		pSprite->m_Rect.X = stream.ReadInt32();
		pSprite->m_Rect.Y = stream.ReadInt32();
		pSprite->m_Rect.FrameWidth = stream.ReadInt32();
		pSprite->m_Rect.FrameHeight = stream.ReadInt32();

		if (pSprite->m_eCompressMode == SgaCompressMode::None) {
			pSprite->m_iDataLength = pSprite->m_Rect.Width * pSprite->m_Rect.Height *
				(pSprite->m_eColorFormat == SgaColorFormat::eArgb8888 ? 4 : 2);
		}

		image.Set(i, spSprite);	// 선 삽입
		pSprite->m_iDataOffset = stream.GetOffset();

		if (indexOnly) {
			stream.Seek(pSprite->m_iDataOffset + pSprite->m_iDataLength);
			continue;
		}

		// 헤더 바로 뒤에 데이터가 위치함
		auto pData = MakeShared<Byte[]>(pSprite->m_iDataLength);
		stream.Read(pData.GetPtr(), 0, pData.Length());
		pSprite->m_spData = pData;
		pSprite->m_bLoaded = true;
		
	}

	image.m_bIndexLoaded = true;
}

void SgaElementInitializerImpl<2>::Initialize(const SgaElementPtr& element, Stream& stream, bool indexOnly)
{
	SgaImage& image = *element.Get<SgaImage*>();
	stream.Seek(image.m_iIndexOffset);
	int iSpriteDataOffset = stream.GetOffset() + element->m_iIndexLength;
	int iWaitForLoadingCount = image.WaitForLoadingDataCount();

	DebugAssertMsg(iWaitForLoadingCount == image.Count(), "로딩해야할 데이터 수와 미리 생성된 NULL 벡터의 용량이랑 다릅니다. (%d:%d)", iWaitForLoadingCount, image.Count());

	for (int i = 0; i < iWaitForLoadingCount; ++i) {
		auto eFormat = (SgaColorFormat)stream.ReadInt32();
		if (eFormat == SgaColorFormat::eLink) {
			auto spLinkSprite = SgaLinkSprite::Create(element, i, eFormat);
			spLinkSprite->m_iTargetFrameIndex = stream.ReadInt32();
			image.Set(i, spLinkSprite);
			continue;
		}

		auto spSprite = SgaSprite::Create(element, i, eFormat);
		auto pSprite = spSprite.GetPtr();
		pSprite->m_eCompressMode = stream.ReadInt32();
		pSprite->m_Rect.Width = stream.ReadInt32();
		pSprite->m_Rect.Height = stream.ReadInt32();
		pSprite->m_iDataLength = stream.ReadInt32();
		pSprite->m_Rect.X = stream.ReadInt32();
		pSprite->m_Rect.Y = stream.ReadInt32();
		pSprite->m_Rect.FrameWidth = stream.ReadInt32();
		pSprite->m_Rect.FrameHeight = stream.ReadInt32();

		if (pSprite->m_eCompressMode == SgaCompressMode::None) {
			pSprite->m_iDataLength = pSprite->GetWidth() * pSprite->GetHeight() *
			pSprite->m_eColorFormat == SgaColorFormat::eArgb8888 ? 4 : 2;
		}

		image.Set(i, spSprite);
	}

	if (stream.GetOffset() < iSpriteDataOffset) {
		image.Clear();
		return;
	}


	for (int i = 0; i < image.Count(); ++i) {
		if (image[i].m_eColorFormat == SgaColorFormat::eLink) 
			continue;

		auto& sprite = (SgaSprite&)image[i];
		sprite.m_iDataOffset = stream.GetOffset();

		if (indexOnly) {
			stream.Seek(sprite.m_iDataOffset + sprite.m_iDataLength);
			continue;
		}

		auto pData = MakeShared<Byte[]>(sprite.m_iDataLength);
		stream.Read(pData.GetPtr(), 0, pData.Length());
		sprite.m_spData = pData;
		sprite.m_bLoaded = true;
	}

	image.m_bIndexLoaded = true;
}


// ============================================================================
//                       버전별 SgaElement 초기화 수행자맵 구성하기
// ============================================================================

using SgaElementInitializerMap = JCore::Vector<SgaElementInitializer*>;
static SgaElementInitializerMap InitializerMap_v {2};

static int MinVersion_v = 0;
static int MaxVersion_v = 2;

void SgaElementInitializer::Initialize() {
	if (InitializerMap_v.Size() != 0)
		return;

	// 인덱스 = 버전이므로 사이에 비어있으면 채워줄 것
	InitializerMap_v.PushBack(dbg_new SgaElementInitializerImpl<0>{});
	InitializerMap_v.PushBack(dbg_new SgaElementInitializerImpl<1>{});
	InitializerMap_v.PushBack(dbg_new SgaElementInitializerImpl<2>{});
}

void SgaElementInitializer::Finalize() {
	for (int i = 0; i < InitializerMap_v.Size(); ++i) {
		DeleteSafe(InitializerMap_v[i]);
	}

	InitializerMap_v.~Vector();
}

void SgaElementInitializer::InitializeElement(const SgaElementPtr& element, Stream& stream, bool indexOnly) {

	int iVersion = element->GetVersion();

	DebugAssertMsg(InitializerMap_v.Size() != 0, "초기화를 먼저 진행해주세요.");
	DebugAssertMsg(iVersion >= MinVersion_v && iVersion <= MaxVersion_v, "올바른 버전이 아닙니다.");
	
	InitializerMap_v[iVersion]->Initialize(element, stream, indexOnly);
}
