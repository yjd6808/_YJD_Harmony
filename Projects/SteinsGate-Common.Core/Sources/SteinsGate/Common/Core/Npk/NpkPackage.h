/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:35:53 AM
 * =====================
 */


#pragma once

#include <SteinsGate/Common/Core/Npk/NpkElement.h>

#include <JCore/Stream.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Primitives/String.h>

class NpkLoader;
class NpkPackage : public JCore::MakeSharedFromThis<NpkPackage>
{
	using NpkPackagePtr = JCore::SharedPtr<NpkPackage>;
public:
	NpkPackage(const JCore::StreamPtr& readOnlyStream,  const JCore::String& path, int capacity)
		: m_szPath(path)
		, m_spStream(readOnlyStream)
		, m_ElementNameToIndex(capacity + 1)
		, m_ElementMap(capacity + 1) {}
	~NpkPackage();

	static NpkPackagePtr Create(const JCore::StreamPtr& readOnlyStream, const JCore::String& path, int capacity);
public:
	const JCore::String& GetPath()			{ return m_szPath; }
	void Add(int idx, const NpkElementPtr& element);
	JCore::StreamPtr Stream()				{ return m_spStream; }
	JCore::Stream& StreamRef()				{ return m_spStream.GetRef(); }
	int Count()								{ return m_ElementMap.Size(); }

	NpkElement& GetAtRef(const int index) { return m_ElementMap[index].GetRef(); }
	void LoadElement(const int index, bool indexOnly);
	void LoadElementPerfect(const int index) { LoadElement(index, false); }
	void LoadElementIndexOnly(const int index) { LoadElement(index, true); }
	bool IsElementLoaded(const int index);
	int GetElementIndex(const JCore::String& elementName);
	bool HasElementIndex(const JCore::String& elementName);

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	NpkElement& operator[](const int idx)	{ return m_ElementMap[idx].GetRef(); }
protected:
	JCore::String m_szPath;
	JCore::StreamPtr m_spStream;

	JCore::HashMap<int, NpkElementPtr> m_ElementMap;

	// Lazy Loading을 위함
	JCore::Vector<NpkElement::Header> m_ElementHeaders;
	JCore::HashMap<JCore::String, int>  m_ElementNameToIndex;

	friend class NpkLoader;
};

using NpkPackagePtr = JCore::SharedPtr<NpkPackage>;


