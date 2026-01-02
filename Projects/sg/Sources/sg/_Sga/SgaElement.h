/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:37:12 AM
 * =====================
 */

#pragma once

#include <jc/Container/Vector.h>

using SgaDataPtr = jc::SharedPtr<Byte[]>;
class SgaLoader;
class SgaPackage;

class SgaElement : public jc::MakeSharedFromThis<SgaElement>
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
		int offset_; // Sga 파일내에서의 위치
		int nextOffset_;
		int indexInPackage_;
		int length_;
		jc::String name_{ 0 };
	};

public:
	SgaElement(Type _type, const Header& _header)
	: type_(_type)
	, header_(_header)
	, isIndexLoaded_(false)
	{
	}

	virtual ~SgaElement() = default;

public:
	Type GetType()
	{
		return type_;
	}

	int GetOffset()
	{
		return header_.offset_;
	}

	int GetLength()
	{
		return header_.length_;
	}

	jc::String& GetName()
	{
		return header_.name_;
	}

	SgaPackage* GetParent();

	Int32 GetVersion()
	{
		return version_;
	}

	Int32 GetIndexOffset()
	{
		return indexOffset_;
	}

	Int32 GetIndexLength()
	{
		return indexLength_;
	}

	const Header& GetHeader()
	{
		return header_;
	}

	bool FullyLoaded()
	{
		return IndexLoaded() && DataLoaded();
	}

	virtual bool IndexLoaded()
	{
		return isIndexLoaded_;
	}

	virtual bool DataLoaded()
	{
		return true;
	}

	virtual bool Load(bool _indexOnly) = 0;

	virtual bool LoadPerfectly()
	{
		return Load(false);
	}

	virtual bool LoadIndexOnly()
	{
		return Load(true);
	}

	virtual bool Unload() = 0;

protected:
	// ========================================
	// 파일에서 읽은 데이터
	// ========================================
	Type type_;
	Header header_;

	Int32 version_ = 0;
	Int32 indexOffset_ = 0; // 인덱스 옵셋 (사운드 데이터는 데이터 시작 위치를 의미)
	Int32 indexLength_ = 0; // 인덱스 길이 (사운드 데이터는 데이터 길이를 의미)

	// ========================================
	// 기본
	// ========================================
	jc::WeakPtr<SgaPackage> pParent_;
	bool isIndexLoaded_;

	template <Int32>
	friend class SgaElementInitializerImpl;
	friend class SgaPackage;
	friend class SgaLoader;
};

using SgaElementPtr = jc::SharedPtr<SgaElement>;
