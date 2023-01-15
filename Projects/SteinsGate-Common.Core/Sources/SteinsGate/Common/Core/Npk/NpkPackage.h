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
class NpkPackage
{
	using NpkPackageElementList = JCore::Vector<NpkElementPtr>;
	using NpkPackagePtr = JCore::SharedPtr<NpkPackage>;
public:
	NpkPackage(const JCore::StreamPtr& readOnlyStream,  const JCore::String& path, int capacity)
		: m_szPath(path)
		, m_spStream(readOnlyStream)
		, m_Elements(capacity + 16)
		, m_bLoaded(false) {}

	static NpkPackagePtr Create(const JCore::StreamPtr& readOnlyStream, const JCore::String& path, int capacity);
public:
	const JCore::String& GetPath()			{ return m_szPath; }
	void Add(const NpkElementPtr& element)	{ m_Elements.PushBack(element); }
	void Remove(const NpkElementPtr& image);
	NpkElementPtr GetAt(const int idx)		{ return m_Elements[idx]; }
	NpkElement& GetAtRef(const int idx)		{ return m_Elements[idx].GetRef(); }
	JCore::StreamPtr Stream()				{ return m_spStream; }
	JCore::Stream& StreamRef()				{ return m_spStream.GetRef(); }
	int Count()								{ return m_Elements.Size(); }
	bool Loaded()							{ return m_bLoaded; }

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	NpkElement& operator[](const int idx)	{ return m_Elements[idx].GetRef(); }
protected:
	JCore::String m_szPath;
	JCore::StreamPtr m_spStream;

	bool m_bLoaded;							// 인덱스만 로딩되었는지, 전체 로딩되었는지 구분치 않음 수행되었는지 여부만 판단가능하다.
	NpkPackageElementList m_Elements;

	friend class NpkLoader;
};

using NpkPackagePtr = JCore::SharedPtr<NpkPackage>;


