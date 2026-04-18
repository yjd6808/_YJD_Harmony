/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:17:04 AM
 * =====================
 *
 */


#pragma once


#include "sg/Sga/SgaSpriteAbstract.h"
#include "jc/Primitives/SmartPtr.h"

#pragma warning (push, 0)
#pragma warning (disable: 26495) // member variable 'Value' is not initialized

class SgaLoader;

class SgaLinkSprite : public SgaSpriteAbstract
{
	using SgaLinkSpritePtr = jc::SharedPtr<SgaLinkSprite>;

public:
	SgaLinkSprite(const SgaImagePtr& _pParent, int _frameIndex, int _format)
	: SgaSpriteAbstract(_pParent, Type::eLinkSprite, _frameIndex, _format)
	{
	}

	~SgaLinkSprite() override;

public:
	int GetTargetFrameIndex() override
	{
		return targetFrameIndex_;
	}

	bool IsLink() override
	{
		return true;
	}

	bool IsDummy() override
	{
		return GetWidth() * GetHeight() == 1 && GetCompressMode() == 5; // eNone
	}

public:
	static SgaLinkSpritePtr Create(const SgaImagePtr& _pParent, int _frameIndex, int _format);

protected:
	int targetFrameIndex_;

	template <_s32>
	friend class SgaElementInitializerImpl;
	friend class SgaLoader;
};

using SgaLinkSpritePtr = jc::SharedPtr<SgaLinkSprite>;

#pragma warning (pop)
