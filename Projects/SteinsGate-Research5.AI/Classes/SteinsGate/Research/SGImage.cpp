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

SGImagePack::SGImagePack(const NpkPackagePtr& npkPackage)
	: m_Package(npkPackage) {
	Initialize();
}

SGImagePack::~SGImagePack() {
	m_ImageMap.Values().Extension().ForEach([](SGImagePack* p) { DeleteSafe(p); });
}

void SGImagePack::Initialize() {

}


// 언젠가 데이터 캐쉬 기능을 개발해야할 듯?
NpkDataPtr SGImagePack::CreateTextureData(int packIndex, int imgIndex, int frameIndex) {
	const char* szPackpath = Packs[packIndex]->GetPath().Source();	// 디버깅용
	NpkImage& img = (NpkImage&)Packs[packIndex]->GetAtRef(imgIndex);
	const char* szImgPath = img.GetName().Source();					// 디버깅용

	if (!img.IndexLoaded() && !img.LoadIndexOnly()) {
		return nullptr;
	}

	if (frameIndex < 0 || frameIndex >= img.Count()) {
		DebugAssert(false);
	}

	NpkSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	if (sprite.IsLink()) {
		return nullptr;
	}

	if (sprite.IsDummy()) {
		return nullptr;
	}

	if (!sprite.Loaded())
		sprite.Load();

	return sprite.Decompress();
}


cocos2d::Texture2D* SGImagePack::CreateTexture(int packIndex, int imgIndex, int frameIndex, NpkSpriteRect& offset) {
	auto spData = CreateTextureData(packIndex, imgIndex, frameIndex);

	if (!spData.Exist())
		return nullptr;

	NpkImage& img = (NpkImage&)Packs[packIndex]->GetAtRef(imgIndex);
	NpkSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	Texture2D* pTexture = new Texture2D();
	pTexture->initWithData(
		spData.GetPtr(),
		spData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(), { sprite.GetWidthF(), sprite.GetHeightF() }
	);
	pTexture->autorelease();
	offset = sprite.GetRect();
	return pTexture;
}

FrameTexturePtr SGImagePack::CreateFrameTexture(int packIndex, int imgIndex, int frameIndex) {
	const char* szPackpath = Packs[packIndex]->GetPath().Source();		// 디버깅용
	NpkImage& img = (NpkImage&)Packs[packIndex]->GetAtRef(imgIndex);
	const char* szImgPath = img.GetName().Source();						// 디버깅용

	if (!img.IndexLoaded() && !img.LoadIndexOnly()) {
		return nullptr;
	}

	if (frameIndex < 0 || frameIndex >= img.Count()) {
		DebugAssert(false);
	}

	NpkSpriteAbstract& sprite = img.GetAtRef(frameIndex);

	if (sprite.IsLink()) {
		return MakeShared<LinkFrameTexture>(sprite.GetTargetFrameIndex());
	}

	if (sprite.IsDummy()) {
		return nullptr;
	}

	if (!sprite.Loaded())
		sprite.Load();

	auto spData = sprite.Decompress();
	Texture2D* pTexture = new Texture2D;
	pTexture->initWithData(
		spData.GetPtr(),
		spData.Length(),
		Texture2D::PixelFormat::RGBA8888,
		sprite.GetWidth(),
		sprite.GetHeight(), { sprite.GetWidthF(), sprite.GetHeightF() }
	);
	pTexture->autorelease();

	return MakeShared<SpriteFrameTexture>(pTexture, sprite.GetRect(), sprite.GetFrameIndex(), sprite.IsDummy());
}
