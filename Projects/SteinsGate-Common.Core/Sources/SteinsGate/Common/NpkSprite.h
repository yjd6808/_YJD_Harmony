/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:00:29 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/NpkSpriteAbstract.h>
#include <SteinsGate/Common/NpkSpriteRect.h>
#include <JCore/Primitives/SmartPtr.h>

#pragma warning (push, 0)
#pragma warning (disable: 26495) // member variable is not initialized


class NpkLoader;
class NpkSprite : public NpkSpriteAbstract
{
	using NpkSpritePtr = JCore::SharedPtr<NpkSprite>;
public:
	NpkSprite(NpkImagePtr parent, int frameIndex, int format)
		: NpkSpriteAbstract(parent, Type::eSprite, frameIndex, format)
		, m_iDataOffset(-1)
		, m_iDataLength(-1)
		, m_bLoaded(false) {}
	~NpkSprite() override;
public:
	bool IsDummy() override			{ return m_Rect.Width * m_Rect.Height <= 10; }
	int GetWidth() override			{ return m_Rect.Width;		  }
	int GetHeight() override		{ return m_Rect.Height;		  }
	int GetX() 	override			{ return m_Rect.X;			  }
	int GetY() 	override			{ return m_Rect.Y;			  }
	int GetFrameWidth() override	{ return m_Rect.FrameWidth;	  }
	int GetFrameHeight() override	{ return m_Rect.FrameHeight;	  }
	int GetCompressMode() override	{ return m_eCompressMode;		  }

	NpkDataPtr GetData() override	{ return m_spData;		  }
	NpkDataPtr Decompress() override;
	NpkSpriteRect GetRect()	override { return m_Rect; }
	

	// ==========================================
	// Lazy Loading을 위함
	// ==========================================
	int GetDataLength() override  { return m_iDataLength; }
	int GetDataOffset() override  { return m_iDataOffset; }
	bool Loaded() override		  { return m_bLoaded;	  }
	bool Load() override;
	void Unload() override;
public:
	static NpkSpritePtr Create(const NpkImagePtr& parent, int frameIndex, int format);
protected:
	int m_eCompressMode;

	NpkSpriteRect m_Rect;
	NpkDataPtr m_spData;				// 이미지 바이너리 데이터

	// ==========================================
	// Lazy Loading을 위함
	// ==========================================
	int m_iDataOffset;
	int m_iDataLength;
	bool m_bLoaded;

	template <Int32>
	friend class NpkElementInitializerImpl;
	friend class NpkLoader;
};

using NpkSpritePtr = JCore::SharedPtr<NpkSprite>;
using NpkSpriteDataPtr = JCore::SharedPtr<Byte[]>;

#pragma warning (pop)