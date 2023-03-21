/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:03:01 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaSpriteRect.h>


#include <SteinsGate/Client/SGConfig.h>
#include "Tutturu.h"

class SGFrameTexture : public cocos2d::Ref
{
public:
	SGFrameTexture(int frameIndex) : m_iFrameIndex(frameIndex) {}
	~SGFrameTexture() override;

	virtual int getWidth()		 = 0;
	virtual int getHeight()		 = 0;
	virtual int getX()			 = 0;
	virtual int getY()			 = 0;
	virtual int getFrameWidth()	 = 0;
	virtual int getFrameHeight() = 0;
	
	virtual int getTargetFrameIndex()		= 0;
	virtual const SgaSpriteRect& getRect()  = 0;

	virtual float getWidthF() 	 	= 0;
	virtual float getHeightF()	 	= 0;
	virtual float getXF()		 	= 0;
	virtual float getYF()		 	= 0;
	virtual float getFrameWidthF() 	= 0;
	virtual float getFrameHeightF() = 0;
	virtual SGTexture* getTexture() = 0;
	virtual bool isLinearDodged()	= 0;
	virtual SGSize getSize()		= 0;
	virtual SGString toString()		= 0;

	virtual bool isLink()  = 0;
	virtual bool isDummy() = 0;

	int getFrameIndex() { return m_iFrameIndex; }
protected:
	int m_iFrameIndex;
};



class SGSpriteFrameTexture : public SGFrameTexture
{
public:
	SGSpriteFrameTexture(SGTexture* texture, const SgaSpriteRect& rect, int frameIndex, bool dummy, bool linearDodge)
		: SGFrameTexture(frameIndex)
		, m_Rect(rect)
		, m_pTexture(texture)
		, m_bDummy(dummy)
		, m_bLinearDodged(linearDodge) {}
	~SGSpriteFrameTexture() override;

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
	const SgaSpriteRect& getRect() 	override { return m_Rect; }
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


class SGLinkFrameTexture : public SGFrameTexture
{
public:
	SGLinkFrameTexture(int frameIndex, int targetFrameIndex)
		: SGFrameTexture(frameIndex)
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

	int getTargetFrameIndex()		 override { return m_iTargetFrameIndex; }
	SGString toString()		 override { return SGStringUtil::Format("프레임(%d) 링크(%d)", m_iFrameIndex, m_iTargetFrameIndex); }

#pragma warning(push, 1)
#pragma warning(disable: 4172) // return local variable address
	const SgaSpriteRect& getRect() 	 override { return {}; }
#pragma warning(pop)
	SGTexture* getTexture() override { return nullptr; }

	bool isLink()  override { return true;	}
	bool isDummy() override { return false;	}
	bool isLinearDodged() override { return false; }
protected:
	int m_iTargetFrameIndex;
};


using FrameTexturePtr = JCore::SharedPtr<SGFrameTexture>;