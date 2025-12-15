/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:03:01 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaSpriteRect.h>

#include <SteinsGate/Client/Tutturu.h>

class FrameTexture : public cocos2d::Ref
{
public:
	FrameTexture(int _frameIndex)
	: frameIndex_(_frameIndex)
	{
	}

	~FrameTexture() override;

	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual int getX() = 0;
	virtual int getY() = 0;
	virtual int getFrameWidth() = 0;
	virtual int getFrameHeight() = 0;

	virtual int getTargetFrameIndex() = 0;
	virtual const SgaSpriteRect& getFullRect() = 0;

	virtual float getWidthF() = 0;
	virtual float getHeightF() = 0;
	virtual float getXF() = 0;
	virtual float getYF() = 0;
	virtual float getFrameWidthF() = 0;
	virtual float getFrameHeightF() = 0;
	virtual SGTexture* getTexture() = 0;
	virtual bool isLinearDodged() = 0;
	virtual SGSize getSize() = 0;
	virtual SGRect getRect() = 0;
	virtual SGString toString() = 0;
	virtual bool isDefaultTexture() const { return false; }

	virtual bool isLink() = 0;
	virtual bool isDummy() = 0;

	int getFrameIndex() { return frameIndex_; }

protected:
	int frameIndex_;
};

class SpriteFrameTexture : public FrameTexture
{
public:
	SpriteFrameTexture(SGTexture* _texture, const SgaSpriteRect& _rect, int _frameIndex, bool _dummy, bool _linearDodge)
	: FrameTexture(_frameIndex)
	, rect_(_rect)
	, pTexture_(_texture)
	, isDummy_(_dummy)
	, isLinearDodged_(_linearDodge)
	{
	}

	~SpriteFrameTexture() override;

	int getWidth() override { return rect_.width_; }
	int getHeight() override { return rect_.height_; }
	int getX() override { return rect_.x_; }
	int getY() override { return rect_.y_; }
	int getFrameWidth() override { return rect_.frameWidth_; }
	int getFrameHeight() override { return rect_.frameHeight_; }

	float getWidthF() override { return (float)rect_.width_; }
	float getHeightF() override { return (float)rect_.height_; }
	float getXF() override { return (float)rect_.x_; }
	float getYF() override { return (float)rect_.y_; }
	float getFrameWidthF() override { return (float)rect_.frameWidth_; }
	float getFrameHeightF() override { return (float)rect_.frameHeight_; }
	SGSize getSize() override { return { (float)rect_.width_, (float)rect_.height_ }; }
	SGString toString() override { return SGStringUtil::Format("프레임(%d)", frameIndex_); }


	int getTargetFrameIndex() override { return frameIndex_; }
	const SgaSpriteRect& getFullRect() override { return rect_; }

	SGRect getRect() override
	{
		return { (float)rect_.x_, (float)rect_.y_, (float)rect_.width_, (float)rect_.height_ };
	}

	SGTexture* getTexture() override { return pTexture_; }

	bool isLink() override { return false; }
	bool isDummy() override { return isDummy_; }
	bool isLinearDodged() override { return isLinearDodged_; }

protected:
	SgaSpriteRect rect_;
	SGTexture* pTexture_;

	bool isDummy_;
	bool isLinearDodged_;
};

//////////////////////////////////////////////////////////////////////////////////////////
class LinkFrameTexture : public FrameTexture
{
public:
	LinkFrameTexture(int _frameIndex, int _targetFrameIndex)
	: FrameTexture(_frameIndex)
	, targetFrameIndex_(_targetFrameIndex)
	{
	}

	int getWidth() override { return 1; }
	int getHeight() override { return 1; }
	int getX() override { return 0; }
	int getY() override { return 0; }
	int getFrameWidth() override { return 0; }
	int getFrameHeight() override { return 1; }

	float getWidthF() override { return 1; }
	float getHeightF() override { return 1; }
	float getXF() override { return 0; }
	float getYF() override { return 0; }
	float getFrameWidthF() override { return 0; }
	float getFrameHeightF() override { return 1; }
	SGSize getSize() override { return { 1, 1 }; }
	SGRect getRect() override { return { 0, 0, 1, 1 }; }

	int getTargetFrameIndex() override { return targetFrameIndex_; }
	SGString toString() override { return SGStringUtil::Format("프레임(%d) 링크(%d)", frameIndex_, targetFrameIndex_); }

#pragma warning(push, 1)
#pragma warning(disable: 4172) // return local variable address
	const SgaSpriteRect& getFullRect() override { return {}; }
#pragma warning(pop)
	SGTexture* getTexture() override { return nullptr; }

	bool isLink() override { return true; }
	bool isDummy() override { return false; }
	bool isLinearDodged() override { return false; }

protected:
	int targetFrameIndex_;
};

class SpriteFrameDefaultTexture : public SpriteFrameTexture
{
public:
	SpriteFrameDefaultTexture(SGTexture* _texture, const SgaSpriteRect& _rect)
	: SpriteFrameTexture(_texture, _rect, 0, false, false)
	{
	}

	bool isDefaultTexture() const override { return true; }
};


using FrameTexturePtr = JCore::SharedPtr<FrameTexture>;
