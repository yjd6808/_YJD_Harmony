/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 7:38:56 AM
 * =====================
 *
 */




#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>
#include <SteinsGate/Common/Core/Npk/NpkImage.h>
#include <SteinsGate/Common/Core/Npk/NpkSprite.h>

SGImagePack::SGImagePack(const NpkPackagePtr& _pNpkPackage)
	: m_Package(_pNpkPackage)
{
	Initialize();
}

SGImagePack::~SGImagePack()
{
	m_ImageMap.Values().Extension().ForEach([](SGImagePack* p) { DeleteSafe(p); });
}

void SGImagePack::Initialize()
{
}

// 언젠가 데이터 캐쉬 기능을 개발해야할 듯?
NpkDataPtr SGImagePack::CreateTextureData(int _packIndex, int _imgIndex, int _frameIndex)
{
	const char* pPackPath = Packs[_packIndex]->GetPath().Source();	// 디버깅용
	NpkImage& image = (NpkImage&)Packs[_packIndex]->GetAtRef(_imgIndex);
	const char* pImagePath = image.GetName().Source();					// 디버깅용

	if (!image.IndexLoaded() && !image.LoadIndexOnly())
	{
		return nullptr;
	}

	if (_frameIndex < 0 || _frameIndex >= image.Count())
	{
		DebugAssert(false);
	}

	NpkSpriteAbstract& sprite = image.GetAtRef(_frameIndex);

	if (sprite.IsLink())
	{
		return nullptr;
	}

	if (sprite.IsDummy())
	{
		return nullptr;
	}

	if (!sprite.Loaded())
	{
		sprite.Load();
	}

	return sprite.Decompress();
}

cocos2d::Texture2D* SGImagePack::CreateTexture(int _packIndex, int _imgIndex, int _frameIndex, NpkSpriteRect& _offset)
{
	auto pData = CreateTextureData(_packIndex, _imgIndex, _frameIndex);

	if (!pData.Exist())
	{
		return nullptr;
	}

	NpkImage& image = (NpkImage&)Packs[_packIndex]->GetAtRef(_imgIndex);
	NpkSpriteAbstract& sprite = image.GetAtRef(_frameIndex);

	Texture2D* pTexture = new Texture2D();
	pTexture->initWithData(
		pData.GetPtr(),
		pData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(),
		{ sprite.GetWidthF(), sprite.GetHeightF() }
	);
	pTexture->autorelease();
	_offset = sprite.GetRect();
	return pTexture;
}

FrameTexturePtr SGImagePack::CreateFrameTexture(int _packIndex, int _imgIndex, int _frameIndex)
{
	const char* pPackPath = Packs[_packIndex]->GetPath().Source();		// 디버깅용
	NpkImage& image = (NpkImage&)Packs[_packIndex]->GetAtRef(_imgIndex);
	const char* pImagePath = image.GetName().Source();						// 디버깅용

	if (!image.IndexLoaded() && !image.LoadIndexOnly())
	{
		return nullptr;
	}

	if (_frameIndex < 0 || _frameIndex >= image.Count())
	{
		DebugAssert(false);
	}

	NpkSpriteAbstract& sprite = image.GetAtRef(_frameIndex);

	if (sprite.IsLink())
	{
		return MakeShared<LinkFrameTexture>(sprite.GetTargetFrameIndex());
	}

	if (sprite.IsDummy())
	{
		return nullptr;
	}

	if (!sprite.Loaded())
	{
		sprite.Load();
	}

	auto pData = sprite.Decompress();
	Texture2D* pTexture = new Texture2D;
	pTexture->initWithData(
		pData.GetPtr(),
		pData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(),
		{ sprite.GetWidthF(), sprite.GetHeightF() }
	);
	pTexture->autorelease();

	return MakeShared<SpriteFrameTexture>(pTexture, sprite.GetRect(), sprite.GetFrameIndex(), sprite.IsDummy());
}
