/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:46:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaImage.h>
#include <SteinsGate/Common/SgaCompressMode.h>
#include <SteinsGate/Common/SgaSpriteRect.h>

template <Int32>
class SgaElementInitializerImpl;
class SgaSpriteAbstract
{
public:
	enum Type
	{
		eSprite,
		eLinkSprite
	};

	SgaSpriteAbstract(const SgaImagePtr& parent, Type type, int frameIndex, int format)
		: m_spParent(parent)
		, m_eType(type)
		, m_iFrameIndex(frameIndex)
		, m_eColorFormat(format) {}

	virtual ~SgaSpriteAbstract() = default;
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
	virtual int GetCompressMode()		{ return SgaCompressMode::Unkown; }
	virtual int GetColorFormat()		{ return m_eColorFormat; }
	virtual bool IsLink()				{ return false; }
	virtual bool IsDummy()				{ return false; }
	virtual SgaDataPtr GetData()		{ return nullptr;  }
	virtual SgaDataPtr Decompress()		{ return nullptr;  }
	virtual SgaSpriteRect GetRect()		{ return {}; }
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
	SgaImageWeakPtr m_spParent;
	Type m_eType;

	int m_iFrameIndex;
	int m_eColorFormat;

	template <Int32>
	friend class SgaElementInitializerImpl;
	friend class SgaLoader;
};

using SgaSpriteAbstractPtr = JCore::SharedPtr<SgaSpriteAbstract>;