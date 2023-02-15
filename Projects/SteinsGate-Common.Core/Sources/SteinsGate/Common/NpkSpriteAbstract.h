/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:46:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/NpkImage.h>
#include <SteinsGate/Common/NpkCompressMode.h>
#include <SteinsGate/Common/NpkSpriteRect.h>

template <Int32>
class NpkElementInitializerImpl;
class NpkSpriteAbstract
{
public:
	enum Type
	{
		eSprite,
		eLinkSprite
	};

	NpkSpriteAbstract(const NpkImagePtr& parent, Type type, int frameIndex, int format)
		: m_spParent(parent)
		, m_eType(type)
		, m_iFrameIndex(frameIndex)
		, m_eColorFormat(format) {}

	virtual ~NpkSpriteAbstract() = default;
public:

	// 타입 캐스팅없이 편하게 사용하기 위해 더미 가상 함수로 둠.
	virtual Type GetType()				{ return m_eType; }
	virtual int GetWidth()				{ return 1; }
	virtual int GetHeight()				{ return 1; }
	virtual int GetX()					{ return 0; }
	virtual int GetY()					{ return 0; }
	virtual int GetLength()				{ return 0; }
	virtual int GetFrameWidth()			{ return 1; }
	virtual int GetFrameHeight()		{ return 1; }
	virtual int GetFrameIndex()			{ return m_iFrameIndex;  }
	virtual int GetTargetFrameIndex()	{ return -1;  }
	virtual int GetCompressMode()		{ return NpkCompressMode::Unkown; }
	virtual int GetColorFormat()		{ return m_eColorFormat; }
	virtual bool IsLink()				{ return false; }
	virtual bool IsDummy()				{ return false; }
	virtual NpkDataPtr GetData()		{ return nullptr;  }
	virtual NpkDataPtr Decompress()		{ return nullptr;  }
	virtual NpkSpriteRect GetRect()		{ return {}; }
	virtual int GetDataOffset()			{ return -1; }
	virtual int GetDataLength()			{ return -1; }
	virtual bool Loaded()				{ return true; }		// LinkSprite는 항상 참
	virtual bool Load()					{ return true;}
	virtual bool Unload()				{ return false; }		// false: 실제로 반환된 메모리가 없는 경우. 링크 스프라이트의 같은넘
	
	float GetXF()						{ return (float)GetX(); }
	float GetYF()						{ return (float)GetY(); }
	float GetWidthF()					{ return (float)GetWidth(); }
	float GetHeightF()					{ return (float)GetHeight(); }
	float GetFrameWidthF()				{ return (float)GetFrameWidth(); }
	float GetFrameHeightF()				{ return (float)GetFrameHeight(); }
	
protected:
	NpkImageWeakPtr m_spParent;
	Type m_eType;

	int m_iFrameIndex;
	int m_eColorFormat;

	template <Int32>
	friend class NpkElementInitializerImpl;
	friend class NpkLoader;
};

using NpkSpriteAbstractPtr = JCore::SharedPtr<NpkSpriteAbstract>;