/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:17:04 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Core/Npk/NpkSpriteAbstract.h>
#include <JCore/Primitives/SmartPtr.h>

#pragma warning (push, 0)
#pragma warning (disable: 26495) // member variable 'Value' is not initialized

class NpkLoader;
class NpkLinkSprite : public NpkSpriteAbstract
{
	using NpkLinkSpritePtr = JCore::SharedPtr<NpkLinkSprite>;
public:
	NpkLinkSprite(const NpkImagePtr& parent, int frameIndex, int format)
		: NpkSpriteAbstract(parent, Type::eLinkSprite, frameIndex, format) {}
	~NpkLinkSprite() override;
public:
	int GetTargetFrameIndex() override { return m_iTargetFrameIndex; }

	bool IsLink() override { return true; }
	bool IsDummy() override { return GetWidth() * GetHeight() == 1 && GetCompressMode() == 5; } // eNone
public:
	static NpkLinkSpritePtr Create(const NpkImagePtr& parent, int frameIndex, int format);
protected:
	int m_iTargetFrameIndex;

	template <Int32>
	friend class NpkElementInitializerImpl;
	friend class NpkLoader;
};

using NpkLinkSpritePtr = JCore::SharedPtr<NpkLinkSprite>;

#pragma warning (pop)
