/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */


#include "PackLoader.h"

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>
#include <SteinsGate/Common/Core/Npk/NpkImage.h>
#include <SteinsGate/Common/Core/Npk/NpkSprite.h>

#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

#include <SteinsGate/FrameTexture.h>


using namespace cocos2d;
using namespace JCore;

void PackLoader::Initialize() {
	NpkElementInitializer::Initialize();

	JCore::Thread PackLoaders[CharacterPartType::Max];
	for (int i = 0; i < CharacterPartType::Max; ++i) {
		PackLoaders[i].Start([i, this](void*) {
			Packs[i] = NpkLoader::LoadIndexOnly(Path::Combine(ImagePackPath, PackPathMap[i]));
			SafeConsole::WriteLine("%d %s 로딩완료", i, PackPathMap[i].Source());
		});
	}

	for (int i = 0; i < CharacterPartType::Max; ++i) {
		PackLoaders[i].Join();
	}
}


// 언젠가 데이터 캐쉬 기능을 개발해야할 듯?
NpkDataPtr PackLoader::CreateTextureData(int packIndex, int imgIndex, int frameIndex) {
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


cocos2d::Texture2D* PackLoader::CreateTexture(int packIndex, int imgIndex, int frameIndex, NpkSpriteRect& offset) {
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

FrameTexturePtr PackLoader::CreateFrameTexture(int packIndex, int imgIndex, int frameIndex) {
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
