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
	SgaPackage(const JCore::StreamPtr& readOnlyStream,  const JCore::String& path, int capacity)
		: m_szPath(path)
		, m_spStream(readOnlyStream)
		, m_ElementNameToIndex(capacity + 1)
		, m_ElementMap(capacity + 1) {}
	~SgaPackage();

	static SgaPackagePtr Create(const JCore::StreamPtr& readOnlyStream, const JCore::String& path, int capacity);
public:
	const JCore::String& GetPath()			{ return m_szPath; }
	void Add(int idx, const SgaElementPtr& element);
	

	JCore::StreamPtr Stream()				{ return m_spStream; }
	JCore::Stream& StreamRef() const		{ return m_spStream.GetRef(); }
	int Count() const						{ return m_ElementMap.Size(); }

	SgaElementPtr Get(int idx);
	SgaElementPtr GetUnsafe(int idx);
	SgaElement& GetAtRef(const int index) { return m_ElementMap[index].GetRef(); }
	void LoadElement(const int index, bool elementOnly);
	void LoadElementIndex(const int index) { LoadElement(index, false); }	// 스프라이트 프레임 인덱스 구성
	void LoadElementOnly(const int index) { LoadElement(index, true); }		// 엘리먼트만 딸랑 만듬, 스프라이트 인덱스 미구성
	int UnloadAllElementData();

	bool IsElementLoaded(const int index) const;
	int GetElementIndex(const JCore::String& elementName);
	bool HasElementIndex(const JCore::String& elementName) const;

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	SgaElement& operator[](const int idx);
	JCore::String ToString() const;
protected:
	JCore::String m_szPath;
	JCore::StreamPtr m_spStream;

	JCore::HashMap<int, SgaElementPtr> m_ElementMap;

	// Lazy Loading을 위함
	JCore::Vector<SgaElement::Header> m_ElementHeaders;
	JCore::HashMap<JCore::String, int>  m_ElementNameToIndex;

	friend class SgaLoader;
};

using SgaPackagePtr = JCore::SharedPtr<SgaPackage>;


