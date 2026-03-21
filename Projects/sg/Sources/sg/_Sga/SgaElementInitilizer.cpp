/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:08:22 PM
 * =====================
 */

#include "sg/_Sga/SgaElementInitializer.h"
#include "sg/_Sga/SgaLinkSprite.h"
#include "sg/_Sga/SgaSprite.h"
#include "sg/_Sga/SgaSound.h"
#include "sg/_Sga/SgaColorFormat.h"

USING_NS_JC;

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializerImpl<-1>::Initialize(const SgaElementPtr& _pElement, Stream& _stream, bool _indexOnly)
{
	jc_assert_msg(false, "이상한 버전이군요");
}

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializerImpl<0>::Initialize(const SgaElementPtr& _pElement, Stream& _stream, bool _indexOnly)
{
	SgaSound& sound = *_pElement.Get<SgaSound*>();
	_stream.Seek(sound.indexOffset_);
	sound.isIndexLoaded_ = true;

	if (_indexOnly)
		return;

	auto pData = MakeShared<_u8[]>(sound.indexLength_);
	_stream.Read(pData.GetPtr(), 0, pData.Length());
	sound.pData_ = pData;
}

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializerImpl<1>::Initialize(const SgaElementPtr& _pElement, Stream& _stream, bool _indexOnly)
{
	SgaImage& image = *_pElement.Get<SgaImage*>();
	_stream.Seek(image.indexOffset_);
	int spriteDataOffset = _stream.GetOffset() + _pElement->indexLength_;
	int waitForLoadingCount = image.WaitForLoadingDataCount();

	jc_assert_msg(waitForLoadingCount == image.Count(), "로딩해야할 데이터 수와 미리 생성된 NULL 벡터의 용량이랑 다릅니다. (%d:%d)",
	               waitForLoadingCount, image.Count());

	for (int i = 0; i < waitForLoadingCount; ++i)
	{
		auto colorFormat = (SgaColorFormat)_stream.ReadInt32();

		if (colorFormat == SgaColorFormat::eLink)
		{
			auto pLinkSpritePtr = SgaLinkSprite::Create(_pElement, i, colorFormat);
			pLinkSpritePtr->targetFrameIndex_ = _stream.ReadInt32();
			image.Set(i, pLinkSpritePtr);
			continue;
		}

		auto pSpritePtr = SgaSprite::Create(_pElement, i, colorFormat);
		auto pSprite = pSpritePtr.GetPtr();

		pSprite->compressMode_ = _stream.ReadInt32();
		pSprite->rect_.width_ = _stream.ReadInt32();
		pSprite->rect_.height_ = _stream.ReadInt32();
		pSprite->dataLength_ = _stream.ReadInt32();
		pSprite->rect_.x_ = _stream.ReadInt32();
		pSprite->rect_.y_ = _stream.ReadInt32();
		pSprite->rect_.frameWidth_ = _stream.ReadInt32();
		pSprite->rect_.frameHeight_ = _stream.ReadInt32();

		if (pSprite->compressMode_ == SgaCompressMode::None)
		{
			pSprite->dataLength_ = pSprite->rect_.width_ * pSprite->rect_.height_ *
				(pSprite->m_eColorFormat == SgaColorFormat::eArgb8888 ? 4 : 2);
		}

		image.Set(i, pSpritePtr); // 선 삽입
		pSprite->dataOffset_ = _stream.GetOffset();

		if (_indexOnly)
		{
			_stream.Seek(pSprite->dataOffset_ + pSprite->dataLength_);
			continue;
		}

		// 헤더 바로 뒤에 데이터가 위치함
		auto pData = MakeShared<_u8[]>(pSprite->dataLength_);
		_stream.Read(pData.GetPtr(), 0, pData.Length());
		pSprite->pData_ = pData;
		pSprite->loaded_ = true;
	}

	image.isIndexLoaded_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializerImpl<2>::Initialize(const SgaElementPtr& _pElement, Stream& _stream, bool _indexOnly)
{
	SgaImage& image = *_pElement.Get<SgaImage*>();
	_stream.Seek(image.indexOffset_);
	int spriteDataOffset = _stream.GetOffset() + _pElement->indexLength_;
	int waitForLoadingCount = image.WaitForLoadingDataCount();

	jc_assert_msg(waitForLoadingCount == image.Count(), "로딩해야할 데이터 수와 미리 생성된 NULL 벡터의 용량이랑 다릅니다. (%d:%d)",
	               waitForLoadingCount, image.Count());

	for (int i = 0; i < waitForLoadingCount; ++i)
	{
		auto colorFormat = (SgaColorFormat)_stream.ReadInt32();

		if (colorFormat == SgaColorFormat::eLink)
		{
			auto pLinkSpritePtr = SgaLinkSprite::Create(_pElement, i, colorFormat);
			pLinkSpritePtr->targetFrameIndex_ = _stream.ReadInt32();
			image.Set(i, pLinkSpritePtr);
			continue;
		}

		auto pSpritePtr = SgaSprite::Create(_pElement, i, colorFormat);
		auto pSprite = pSpritePtr.GetPtr();

		pSprite->compressMode_ = _stream.ReadInt32();
		pSprite->rect_.width_ = _stream.ReadInt32();
		pSprite->rect_.height_ = _stream.ReadInt32();
		pSprite->dataLength_ = _stream.ReadInt32();
		pSprite->rect_.x_ = _stream.ReadInt32();
		pSprite->rect_.y_ = _stream.ReadInt32();
		pSprite->rect_.frameWidth_ = _stream.ReadInt32();
		pSprite->rect_.frameHeight_ = _stream.ReadInt32();

		if (pSprite->compressMode_ == SgaCompressMode::None)
		{
			pSprite->dataLength_ = pSprite->GetWidth() * pSprite->GetHeight() *
				(pSprite->m_eColorFormat == SgaColorFormat::eArgb8888 ? 4 : 2);
		}

		image.Set(i, pSpritePtr);
	}

	if (_stream.GetOffset() < spriteDataOffset)
	{
		image.Clear();
		return;
	}

	for (int i = 0; i < image.Count(); ++i)
	{
		if (image[i].m_eColorFormat == SgaColorFormat::eLink)
			continue;

		auto& sprite = (SgaSprite&)image[i];
		sprite.dataOffset_ = _stream.GetOffset();

		if (_indexOnly)
		{
			_stream.Seek(sprite.dataOffset_ + sprite.dataLength_);
			continue;
		}

		auto pData = MakeShared<_u8[]>(sprite.dataLength_);
		_stream.Read(pData.GetPtr(), 0, pData.Length());
		sprite.pData_ = pData;
		sprite.loaded_ = true;
	}

	image.isIndexLoaded_ = true;
}

// ============================================================================
//                       버전별 SgaElement 초기화 수행자맵 구성하기
// ============================================================================

using SgaElementInitializerMap = Vector<SgaElementInitializer*>;
static SgaElementInitializerMap InitializerMap_v{ 2 };

static int MinVersion_v = 0;
static int MaxVersion_v = 2;

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializer::Initialize()
{
	if (InitializerMap_v.Size() != 0)
		return;

	// 인덱스 = 버전이므로 사이에 비어있으면 채워줄 것
	InitializerMap_v.PushBack(dbg_new SgaElementInitializerImpl<0>{});
	InitializerMap_v.PushBack(dbg_new SgaElementInitializerImpl<1>{});
	InitializerMap_v.PushBack(dbg_new SgaElementInitializerImpl<2>{});
}

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializer::Finalize()
{
	for (int i = 0; i < InitializerMap_v.Size(); ++i)
	{
		JC_DELETE_SAFE(InitializerMap_v[i]);
	}

	InitializerMap_v.~Vector();
}

////////////////////////////////////////////////////////////////////////////////////////
void SgaElementInitializer::InitializeElement(const SgaElementPtr& _pElement, Stream& _stream, bool _indexOnly)
{
	int version = _pElement->GetVersion();

	jc_assert_msg(InitializerMap_v.Size() != 0, "초기화를 먼저 진행해주세요.");
	jc_assert_msg(version >= MinVersion_v && version <= MaxVersion_v, "올바른 버전이 아닙니다.");

	InitializerMap_v[version]->Initialize(_pElement, _stream, _indexOnly);
}
