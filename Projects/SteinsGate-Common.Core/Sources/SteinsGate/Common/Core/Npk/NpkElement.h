/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:37:12 AM
 * =====================
 */


#pragma once

#include <JCore/Container/Vector.h>

using NpkDataPtr = JCore::SharedPtr<Byte[]>;
class NpkLoader;
class NpkPackage;
class NpkElement : public JCore::MakeSharedFromThis<NpkElement>
{
public:
	enum Type
	{
		eUnknown,
		eImage,
		eSound,
	};

	struct Header
	{
		int Offset;			// Npk 파일내에서의 위치
		int NextOffset;
		int IndexInPackage;
		int Length;
		JCore::String Name{ 0 };
	};

public:
	NpkElement(Type type, const Header& header)
		: m_eType(type)
		, m_Header(header)
		, m_bIndexLoaded(false) {}

	virtual ~NpkElement() = default;
public:
	Type GetType() { return m_eType; }
	int GetOffset() { return m_Header.Offset;  }
	int GetLength() { return m_Header.Length;  }
	JCore::String& GetName() { return m_Header.Name; }
	NpkPackage* GetParent();

	Int32 GetVersion() { return m_iVersion; }
	Int32 GetIndexOffset() { return m_iIndexOffset; }
	Int32 GetIndexLength() { return m_iIndexLength; }
	const Header& GetHeader() { return m_Header; }


	bool FullyLoaded() { return IndexLoaded() && DataLoaded(); }
	virtual bool IndexLoaded() { return m_bIndexLoaded; }
	virtual bool DataLoaded() { return true;  }
	

	virtual bool Load(bool indexOnly) = 0;
	virtual bool LoadPerfectly() { return Load(false); }
	virtual bool LoadIndexOnly() { return Load(true); }
	virtual void Unload() = 0;
protected:

	// ========================================
	// 파일에서 읽은 데이터
	// ========================================
	Type m_eType;
	Header m_Header;

	Int32 m_iVersion = 0;
	Int32 m_iIndexOffset = 0;		// 인덱스 옵셋 (사운드 데이터는 데이터 시작 위치를 의미)
	Int32 m_iIndexLength = 0;		// 인덱스 길이 (사운드 데이터는 데이터 길이를 의미)

	// ========================================
	// 기본
	// ========================================
	JCore::WeakPtr<NpkPackage> m_spParent;
	bool m_bIndexLoaded;

	template <Int32>
	friend class NpkElementInitializerImpl;
	friend class NpkPackage;
	friend class NpkLoader;
};

using NpkElementPtr = JCore::SharedPtr<NpkElement>;