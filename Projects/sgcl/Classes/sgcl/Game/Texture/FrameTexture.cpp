/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 4:29:55 AM
 * =====================
 *
 */

#include "Game/Texture/FrameTexture.h"


namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	SpriteFrameDefaultTexture* CreateDefaultFrameTexture()
	{
		cc::Texture* pTexture = cc::Texture::create2x2DefaultTexture();
		SgaSpriteRect rect{ 0, 0, 2, 2, 2, 2 };
		auto pDefaultTexture = dbg_new SpriteFrameDefaultTexture(pTexture, rect);
		pDefaultTexture->autorelease();
		pDefaultTexture->retain();
		return pDefaultTexture;
	}

	SpriteFrameDefaultTexture* DefaultTexture;
}


//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture::~FrameTexture() 
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SpriteFrameTexture::~SpriteFrameTexture() 
{
	_LogDebug_("%d 삭제", frameIndex_);
	JC_DELETE_SAFE(pTexture_);
}

//////////////////////////////////////////////////////////////////////////////////////////
SpriteFrameTexture* SpriteFrameTexture::GetDefault()
{
	if (DefaultTexture == nullptr)
	{
		DefaultTexture = CreateDefaultFrameTexture(); // OpenGL 컨텍스트가 생성된 이후에 호출되어야 함
	}

	return DefaultTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SpriteFrameTexture::FreeDefault()
{
	CC_SAFE_RELEASE(DefaultTexture);
}

