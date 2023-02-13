/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:08:22 PM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/NpkElementInitializer.h>
#include <SteinsGate/Common/NpkLinkSprite.h>
#include <SteinsGate/Common/NpkSprite.h>
#include <SteinsGate/Common/NpkSound.h>
#include <SteinsGate/Common/NpkColorFormat.h>

using namespace JCore;

void NpkElementInitializerImpl<-1>::Initialize(const NpkElementPtr& element, Stream& stream, bool indexOnly)
{
	DebugAssertMsg(false, "이상한 버전이군요");
}

void NpkElementInitializerImpl<0>::Initialize(const NpkElementPtr& element, Stream& stream, bool indexOnly)
{
	
	NpkSound& sound = *element.Get<NpkSound*>();
	stream.Seek(sound.m_iIndexOffset);
	sound.m_bIndexLoaded = true;
	if (indexOnly) return;
	auto pData = MakeShared<Byte[]>(sound.m_iIndexLength);
	stream.Read(pData.GetPtr(), 0, pData.Length());
	sound.m_spData = pData;
}

void NpkElementInitializerImpl<2>::Initialize(const NpkElementPtr& element, Stream& stream, bool indexOnly)
{
	NpkImage& image = *element.Get<NpkImage*>();
	stream.Seek(image.m_iIndexOffset);
	int iSpriteDataOffset = stream.GetOffset() + element->m_iIndexLength;
	

	for (int i = 0; i < image.WaitForLoadingDataCount(); ++i) {
		auto eFormat = (NpkColorFormat)stream.ReadInt32();
		if (eFormat == NpkColorFormat::eLink) {
			auto spLinkSprite = NpkLinkSprite::Create(element, i, eFormat);
			spLinkSprite->m_iTargetFrameIndex = stream.ReadInt32();
			image.Add(spLinkSprite);
			continue;
		}

		auto spSprite = NpkSprite::Create(element, i, eFormat);
		spSprite->m_eCompressMode = stream.ReadInt32();
		spSprite->m_Rect.Width = stream.ReadInt32();
		spSprite->m_Rect.Height = stream.ReadInt32();
		spSprite->m_iDataLength = stream.ReadInt32();
		spSprite->m_Rect.X = stream.ReadInt32();
		spSprite->m_Rect.Y = stream.ReadInt32();
		spSprite->m_Rect.FrameWidth = stream.ReadInt32();
		spSprite->m_Rect.FrameHeight = stream.ReadInt32();

		if (spSprite->m_eCompressMode == NpkCompressMode::None) {
			spSprite->m_iDataLength = spSprite->GetWidth() * spSprite->GetHeight() *
			spSprite->m_eColorFormat == NpkColorFormat::eArgb8888 ? 4 : 2;
		}

		image.Add(spSprite);
	}

	if (stream.GetOffset() < iSpriteDataOffset) {
		image.Clear();
		return;
	}


	for (int i = 0; i < image.Count(); ++i) {
		if (image[i].m_eColorFormat == NpkColorFormat::eLink) 
			continue;

		auto& sprite = (NpkSprite&)image[i];
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
}


// ============================================================================
//                       버전별 NpkElement 초기화 수행자맵 구성하기
// ============================================================================

using NpkElementInitializerMap = JCore::Vector<NpkElementInitializer*>;
static NpkElementInitializerMap InitializerMap_v {2};

static int MinVersion_v = 0;
static int MaxVersion_v = 2;

void NpkElementInitializer::Initialize() {
	if (InitializerMap_v.Size() != 0)
		return;

	// 인덱스 = 버전이므로 사이에 비어있으면 채워줄 것
	InitializerMap_v.PushBack(new NpkElementInitializerImpl<0>{});
	InitializerMap_v.PushBack(new NpkElementInitializerImpl<-1>{});
	InitializerMap_v.PushBack(new NpkElementInitializerImpl<2>{});
}

void NpkElementInitializer::Finalize() {
	for (int i = 0; i < InitializerMap_v.Size(); ++i) {
		delete InitializerMap_v[i];
	}

	InitializerMap_v.~Vector();
}

void NpkElementInitializer::InitializeElement(const NpkElementPtr& element, Stream& stream, bool indexOnly) {

	int iVersion = element->GetVersion();

	DebugAssertMsg(InitializerMap_v.Size() != 0, "초기화를 먼저 진행해주세요.");
	DebugAssertMsg(iVersion >= MinVersion_v && iVersion <= MaxVersion_v, "올바른 버전이 아닙니다.");
	
	InitializerMap_v[iVersion]->Initialize(element, stream, indexOnly);
}
