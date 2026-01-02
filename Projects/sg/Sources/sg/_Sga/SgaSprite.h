/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:00:29 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Sga/SgaSpriteAbstract.h>
#include <sg/_Sga/SgaSpriteRect.h>
#include <jc/Primitives/SmartPtr.h>

#pragma warning (push, 0)
#pragma warning (disable: 26495) // member variable is not initialized


class SgaLoader;

class SgaSprite : public SgaSpriteAbstract
{
	using SgaSpritePtr = jc::SharedPtr<SgaSprite>;

public:
	SgaSprite(const SgaImagePtr& _pParent, int _frameIndex, int _format)
	: SgaSpriteAbstract(_pParent, Type::eSprite, _frameIndex, _format)
	, compressMode_(0)
	, rect_()
	, dataOffset_(-1)
	, dataLength_(-1)
	, loaded_(false)
	{
	}

	~SgaSprite() override;

public:
	bool IsDummy() override { return rect_.width_ * rect_.height_ <= 10; }
	int GetWidth() override { return rect_.width_; }
	int GetHeight() override { return rect_.height_; }
	int GetX() override { return rect_.x_; }
	int GetY() override { return rect_.y_; }
	int GetFrameWidth() override { return rect_.frameWidth_; }
	int GetFrameHeight() override { return rect_.frameHeight_; }
	int GetCompressMode() override { return compressMode_; }

	SgaDataPtr GetData() override { return pData_; }
	SgaDataPtr Decompress() override;

	SgaSpriteRect GetRect() override { return rect_; }

	// ==========================================
	// Lazy Loading을 위함
	// ==========================================
	int GetDataLength() override { return dataLength_; }
	int GetDataOffset() override { return dataOffset_; }
	bool Loaded() override { return loaded_; }
	bool Load() override;
	bool Unload() override;

public:
	static SgaSpritePtr Create(const SgaImagePtr& _pParent, int _frameIndex, int _format);

protected:
	int compressMode_;

	SgaSpriteRect rect_;
	SgaDataPtr pData_;				// 이미지 바이너리 데이터

	// ==========================================
	// Lazy Loading을 위함
	// ==========================================
	int dataOffset_;
	int dataLength_;
	bool loaded_;

	template <Int32>
	friend class SgaElementInitializerImpl;
	friend class SgaLoader;
};

using SgaSpritePtr = jc::SharedPtr<SgaSprite>;
using SgaSpriteDataPtr = jc::SharedPtr<Byte[]>;

#pragma warning (pop)
