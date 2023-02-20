/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:17:04 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/SgaSpriteAbstract.h>
#include <JCore/Primitives/SmartPtr.h>

#pragma warning (push, 0)
#pragma warning (disable: 26495) // member variable 'Value' is not initialized

class SgaLoader;
class SgaLinkSprite : public SgaSpriteAbstract
{
	using SgaLinkSpritePtr = JCore::SharedPtr<SgaLinkSprite>;
public:
	SgaLinkSprite(const SgaImagePtr& parent, int frameIndex, int format)
		: SgaSpriteAbstract(parent, Type::eLinkSprite, frameIndex, format) {}
	~SgaLinkSprite() override;
public:
	int GetTargetFrameIndex() override { return m_iTargetFrameIndex; }

	bool IsLink() override { return true; }
	bool IsDummy() override { return GetWidth() * GetHeight() == 1 && GetCompressMode() == 5; } // eNone
public:
	static SgaLinkSpritePtr Create(const SgaImagePtr& parent, int frameIndex, int format);
protected:
	int m_iTargetFrameIndex;

	template <Int32>
	friend class SgaElementInitializerImpl;
	friend class SgaLoader;
};

using SgaLinkSpritePtr = JCore::SharedPtr<SgaLinkSprite>;

#pragma warning (pop)
