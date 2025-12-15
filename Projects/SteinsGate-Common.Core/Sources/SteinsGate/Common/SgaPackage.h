/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:35:53 AM
 * =====================
 */


#pragma once

#include <SteinsGate/Common/SgaElement.h>

#include <JCore/Stream.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Primitives/String.h>

class SgaLoader;

class SgaPackage : public JCore::MakeSharedFromThis<SgaPackage>
{
	using SgaPackagePtr = JCore::SharedPtr<SgaPackage>;

public:
	SgaPackage(const JCore::StreamPtr& _pReadOnlyStream, const JCore::String& _path, int _capacity)
	: path_(_path)
	, stream_(_pReadOnlyStream)
	, elementMap_(_capacity + 1)
	, elementNameToIndex_(_capacity + 1)
	{
	}

	~SgaPackage();

	static SgaPackagePtr Create(const JCore::StreamPtr& _pReadOnlyStream, const JCore::String& _path, int _capacity);

public:
	const JCore::String& GetPath() { return path_; }

	void Add(int _index, const SgaElementPtr& _pElement);

	JCore::StreamPtr Stream() { return stream_; }
	JCore::Stream& StreamRef() const { return stream_.GetRef(); }
	int Count() const { return elementMap_.Size(); }

	SgaElementPtr Get(int _index);
	SgaElementPtr GetUnsafe(int _index);
	SgaElement& GetAtRef(const int _index) { return elementMap_[_index].GetRef(); }

	void LoadElement(const int _index, bool _elementOnly);
	void LoadElementIndex(const int _index) { LoadElement(_index, false); } // 스프라이트 프레임 인덱스 구성
	void LoadElementOnly(const int _index) { LoadElement(_index, true); } // 엘리먼트만 딸랑 만듬, 스프라이트 인덱스 미구성
	int UnloadAllElementData();

	bool IsElementLoaded(const int _index) const;

	int GetElementIndex(const JCore::String& _elementName);
	int GetElementIndex(const char* _elementName);

	bool HasElementIndex(const JCore::String& _elementName) const;
	bool HasElementIndex(const char* _elementName) const;

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	SgaElement& operator[](const int _index);
	JCore::String ToString() const;
protected:
	JCore::String path_;
	JCore::StreamPtr stream_;

	JCore::HashMap<int, SgaElementPtr> elementMap_;

	// Lazy Loading을 위함
	JCore::Vector<SgaElement::Header> elementHeaders_;
	JCore::HashMap<JCore::String, int> elementNameToIndex_;

	friend class SgaLoader;
};

using SgaPackagePtr = JCore::SharedPtr<SgaPackage>;
