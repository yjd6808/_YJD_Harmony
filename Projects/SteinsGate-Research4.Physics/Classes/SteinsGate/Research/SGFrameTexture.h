/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:03:01 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Core/Npk/NpkSpriteRect.h>


#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/Tutturu.h>

class SGFrameTexture : public cocos2d::Ref
{
public:
	~SGFrameTexture() override { Log("텍스쳐 소멸"); }

	virtual int getWidth()		 = 0;
	virtual int getHeight()		 = 0;
	virtual int getX()			 = 0;
	virtual int getY()			 = 0;
	virtual int getFrameWidth()	 = 0;
	virtual int getFrameHeight() = 0;
	virtual int getFrameIndex()	 = 0;
	virtual int getTargetFrameIndex()		= 0;
	virtual const NpkSpriteRect& getRect()  = 0;

	virtual float getWidthF() 	 	= 0;
	virtual float getHeightF()	 	= 0;
	virtual float getXF()		 	= 0;
	virtual float getYF()		 	= 0;
	virtual float getFrameWidthF() 	= 0;
	virtual float getFrameHeightF() = 0;
	virtual cocos2d::Texture2D* getTexture() = 0;

	virtual bool isLink()  = 0;
	virtual bool isDummy() = 0;
};

class SGSpriteFrameTexture : public SGFrameTexture
{
public:
	SGSpriteFrameTexture(cocos2d::Texture2D* texture, const NpkSpriteRect& rect, int frameIndex, bool dummy)
		: SGFrameTexture()
		, m_Rect(rect)
		, m_pTexture(texture)
		, m_iFrameIndex(frameIndex)
		, m_bDummy(dummy) {}

	int getWidth()		 override { return m_Rect.Width;  }
	int getHeight()		 override { return m_Rect.Height; }
	int getX()			 override { return m_Rect.X; }
	int getY()			 override { return m_Rect.Y; }
	int getFrameWidth()	 override { return m_Rect.FrameWidth;	}
	int getFrameHeight() override { return m_Rect.FrameHeight;	}

	float getWidthF()		 override { return (float)m_Rect.Width;  }
	float getHeightF()		 override { return (float)m_Rect.Height; }
	float getXF()			 override { return (float)m_Rect.X; }
	float getYF()			 override { return (float)m_Rect.Y; }
	float getFrameWidthF()	 override { return (float)m_Rect.FrameWidth;	}
	float getFrameHeightF()  override { return (float)m_Rect.FrameHeight;	}

	int getFrameIndex()				override { return m_iFrameIndex; }
	int getTargetFrameIndex()		override { return m_iFrameIndex; }
	const NpkSpriteRect& getRect() 	override { return m_Rect; }
	cocos2d::Texture2D* getTexture() { return m_pTexture; }

	bool isLink()  override { return false;		}
	bool isDummy() override { return m_bDummy;	}
protected:
	NpkSpriteRect m_Rect;
	cocos2d::Texture2D* m_pTexture;
	int m_iFrameIndex;
	bool m_bDummy;
};


class SGLinkFrameTexture : public SGFrameTexture
{
public:
	SGLinkFrameTexture(int targetFrameIndex)
		: SGFrameTexture()
		, m_iTargetFrameIndex(targetFrameIndex) {}

	int getWidth()		 override { return 1; }
	int getHeight()		 override { return 1; }
	int getX()			 override { return 0; }
	int getY()			 override { return 0; }
	int getFrameWidth()	 override { return 0; }
	int getFrameHeight() override { return 1; }

	float getWidthF()		 override { return 1; }
	float getHeightF()		 override { return 1; }
	float getXF()			 override { return 0; }
	float getYF()			 override { return 0; }
	float getFrameWidthF()	 override { return 0; }
	float getFrameHeightF()  override { return 1; }

	int getFrameIndex()				 override { return InvalidIndex_v; }
	int getTargetFrameIndex()		 override { return m_iTargetFrameIndex; }
	const NpkSpriteRect& getRect() 	 override { return {}; }
	cocos2d::Texture2D* getTexture() override { return nullptr; }

	bool isLink()  override { return true;	}
	bool isDummy() override { return false;	}
protected:
	int m_iTargetFrameIndex;
};


using FrameTexturePtr = JCore::SharedPtr<SGFrameTexture>;