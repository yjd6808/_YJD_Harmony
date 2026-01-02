/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:03:01 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Sga/SgaSpriteRect.h>

#include <sgcl/Core.h>

class FrameTexture : public cocos2d::Ref
{
public:
	FrameTexture(int _frameIndex)
	: frameIndex_(_frameIndex)
	{
	}

	~FrameTexture() override;

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual int GetX() = 0;
	virtual int GetY() = 0;
	virtual int GetFrameWidth() = 0;
	virtual int GetFrameHeight() = 0;

	virtual int GetTargetFrameIndex() = 0;
	virtual const SgaSpriteRect& GetFullRect() = 0;

	virtual float GetWidthF() = 0;
	virtual float GetHeightF() = 0;
	virtual float GetXF() = 0;
	virtual float GetYF() = 0;
	virtual float GetFrameWidthF() = 0;
	virtual float GetFrameHeightF() = 0;
	virtual cc::Texture* GetTexture() = 0;
	virtual bool IsLinearDodged() = 0;
	virtual cc::size GetSize() = 0;
	virtual cc::rect GetRect() = 0;
	virtual jc::String ToString() = 0;
	virtual bool IsDefaultTexture() const { return false; }

	virtual bool IsLink() = 0;
	virtual bool IsDummy() = 0;

	int GetFrameIndex() { return frameIndex_; }

protected:
	int frameIndex_;
};

class SpriteFrameTexture : public FrameTexture
{
public:
	SpriteFrameTexture(cc::Texture* _texture, const SgaSpriteRect& _rect, int _frameIndex, bool _dummy, bool _linearDodge)
	: FrameTexture(_frameIndex)
	, rect_(_rect)
	, pTexture_(_texture)
	, isDummy_(_dummy)
	, isLinearDodged_(_linearDodge)
	{
	}

	~SpriteFrameTexture() override;

	int GetWidth() override { return rect_.width_; }
	int GetHeight() override { return rect_.height_; }
	int GetX() override { return rect_.x_; }
	int GetY() override { return rect_.y_; }
	int GetFrameWidth() override { return rect_.frameWidth_; }
	int GetFrameHeight() override { return rect_.frameHeight_; }

	float GetWidthF() override { return (float)rect_.width_; }
	float GetHeightF() override { return (float)rect_.height_; }
	float GetXF() override { return (float)rect_.x_; }
	float GetYF() override { return (float)rect_.y_; }
	float GetFrameWidthF() override { return (float)rect_.frameWidth_; }
	float GetFrameHeightF() override { return (float)rect_.frameHeight_; }
	cc::size GetSize() override { return { (float)rect_.width_, (float)rect_.height_ }; }
	jc::String ToString() override { return jc::StringUtil::Format("프레임(%d)", frameIndex_); }


	int GetTargetFrameIndex() override { return frameIndex_; }
	const SgaSpriteRect& GetFullRect() override { return rect_; }

	cc::rect GetRect() override
	{
		return { (float)rect_.x_, (float)rect_.y_, (float)rect_.width_, (float)rect_.height_ };
	}

	cc::Texture* GetTexture() override { return pTexture_; }

	bool IsLink() override { return false; }
	bool IsDummy() override { return isDummy_; }
	bool IsLinearDodged() override { return isLinearDodged_; }

protected:
	SgaSpriteRect rect_;
	cc::Texture* pTexture_;

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

	int GetWidth() override { return 1; }
	int GetHeight() override { return 1; }
	int GetX() override { return 0; }
	int GetY() override { return 0; }
	int GetFrameWidth() override { return 0; }
	int GetFrameHeight() override { return 1; }

	float GetWidthF() override { return 1; }
	float GetHeightF() override { return 1; }
	float GetXF() override { return 0; }
	float GetYF() override { return 0; }
	float GetFrameWidthF() override { return 0; }
	float GetFrameHeightF() override { return 1; }
	cc::size GetSize() override { return { 1, 1 }; }
	cc::rect GetRect() override { return { 0, 0, 1, 1 }; }

	int GetTargetFrameIndex() override { return targetFrameIndex_; }
	jc::String ToString() override { return jc::StringUtil::Format("프레임(%d) 링크(%d)", frameIndex_, targetFrameIndex_); }

#pragma warning(push, 1)
#pragma warning(disable: 4172) // return local variable address
	const SgaSpriteRect& GetFullRect() override { return {}; }
#pragma warning(pop)
	cc::Texture* GetTexture() override { return nullptr; }

	bool IsLink() override { return true; }
	bool IsDummy() override { return false; }
	bool IsLinearDodged() override { return false; }

protected:
	int targetFrameIndex_;
};

class SpriteFrameDefaultTexture : public SpriteFrameTexture
{
public:
	SpriteFrameDefaultTexture(cc::Texture* _texture, const SgaSpriteRect& _rect)
	: SpriteFrameTexture(_texture, _rect, 0, false, false)
	{
	}

	bool IsDefaultTexture() const override { return true; }
};


using FrameTexturePtr = jc::SharedPtr<FrameTexture>;
