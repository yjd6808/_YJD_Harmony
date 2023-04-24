/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:03:01 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaSpriteRect.h>


#include <SteinsGate/Client/Config.h>
#include "Tutturu.h"

class FrameTexture : public cocos2d::Ref
{
public:
	FrameTexture(int frameIndex) : m_iFrameIndex(frameIndex) {}
	~FrameTexture() override;

	virtual int getWidth()		 = 0;
	virtual int getHeight()		 = 0;
	virtual int getX()			 = 0;
	virtual int getY()			 = 0;
	virtual int getFrameWidth()	 = 0;
	virtual int getFrameHeight() = 0;

	virtual int getTargetFrameIndex()			= 0;
	virtual const SgaSpriteRect& getFullRect()  = 0;

	virtual float getWidthF() 	 	= 0;
	virtual float getHeightF()	 	= 0;
	virtual float getXF()		 	= 0;
	virtual float getYF()		 	= 0;
	virtual float getFrameWidthF() 	= 0;
	virtual float getFrameHeightF() = 0;
	virtual SGTexture* getTexture() = 0;
	virtual bool isLinearDodged()	= 0;
	virtual SGSize getSize()		= 0;
	virtual SGRect getRect()		= 0;
	virtual SGString toString()		= 0;
	virtual bool isDefaultTexture() const { return false; }

	virtual bool isLink()  = 0;
	virtual bool isDummy() = 0;

	int getFrameIndex() { return m_iFrameIndex; }
protected:
	int m_iFrameIndex;
};

class SpriteFrameTexture : public FrameTexture
{
public:
	SpriteFrameTexture(SGTexture* texture, const SgaSpriteRect& rect, int frameIndex, bool dummy, bool linearDodge)
		: FrameTexture(frameIndex)
		, m_Rect(rect)
		, m_pTexture(texture)
		, m_bDummy(dummy)
		, m_bLinearDodged(linearDodge) {}
	~SpriteFrameTexture() override;

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
	SGSize getSize()		 override { return { (float)m_Rect.Width, (float)m_Rect.Height }; }
	SGString toString()		 override { return SGStringUtil::Format("프레임(%d)", m_iFrameIndex); }
	

	int getTargetFrameIndex()		override { return m_iFrameIndex; }
	const SgaSpriteRect& getFullRect() 	override { return m_Rect; }
	SGRect getRect() override { return { (float)m_Rect.X, (float)m_Rect.Y, (float)m_Rect.Width, (float)m_Rect.Height }; }
	SGTexture* getTexture() { return m_pTexture; }

	bool isLink()  override { return false;		}
	bool isDummy() override { return m_bDummy;	}
	bool isLinearDodged() override { return m_bLinearDodged; }
protected:
	SgaSpriteRect m_Rect;
	SGTexture* m_pTexture;
	
	bool m_bDummy;
	bool m_bLinearDodged;
};


class LinkFrameTexture : public FrameTexture
{
public:
	LinkFrameTexture(int frameIndex, int targetFrameIndex)
		: FrameTexture(frameIndex)
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
	SGSize getSize()		 override { return { 1, 1 }; }
	SGRect getRect() override { return { 0, 0, 1, 1 }; }

	int getTargetFrameIndex()		 override { return m_iTargetFrameIndex; }
	SGString toString()		 override { return SGStringUtil::Format("프레임(%d) 링크(%d)", m_iFrameIndex, m_iTargetFrameIndex); }

#pragma warning(push, 1)
#pragma warning(disable: 4172) // return local variable address
	const SgaSpriteRect& getFullRect() 	 override { return {}; }
#pragma warning(pop)
	SGTexture* getTexture() override { return nullptr; }

	bool isLink()  override { return true;	}
	bool isDummy() override { return false;	}
	bool isLinearDodged() override { return false; }
protected:
	int m_iTargetFrameIndex;
};

class SpriteFrameDefaultTexture : public SpriteFrameTexture
{
public:
	SpriteFrameDefaultTexture(SGTexture* texture, const SgaSpriteRect& rect)
		: SpriteFrameTexture(texture, rect, 0, false, false) {}
	bool isDefaultTexture() const override { return true; }
};


using FrameTexturePtr = JCore::SharedPtr<FrameTexture>;