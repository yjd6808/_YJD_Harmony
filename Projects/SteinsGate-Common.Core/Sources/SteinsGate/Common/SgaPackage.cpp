/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:36:58 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/SgaLoader.h>
#include <SteinsGate/Common/SgaPackage.h>

#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Path.h>


USING_NS_JC;

SgaPackage::~SgaPackage() {}

SgaPackagePtr SgaPackage::Create(const JCore::StreamPtr& readOnlyStream, const String& path, int capacity) {
	return MakeShared<SgaPackage>(readOnlyStream, path, capacity);
}

void SgaPackage::Add(const int idx, const SgaElementPtr& element) {
	m_ElementMap.Insert(idx, element);
}

SgaElementPtr SgaPackage::Get(int idx) {
	DebugAssertMsg(m_ElementMap.Exist(idx), "엘리먼트가 존재하지 않습니다.");
	return m_ElementMap[idx];
}

SgaElementPtr SgaPackage::GetUnsafe(int idx) {
	if (!m_ElementMap.Exist(idx))
		return nullptr;

	return m_ElementMap[idx];
}

void SgaPackage::LoadElement(const int index, bool elementOnly) {
	DebugAssertMsg(index >= 0 && index < m_ElementHeaders.Size(), "올바르지 않은 Element 인덱스입니다.");
	const SgaElementPtr spElement = SgaLoader::ReadElement(m_spStream.GetRef(), m_ElementHeaders[index], m_ElementHeaders[index].NextOffset, elementOnly);
	DebugAssertMsg(spElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
	spElement->m_spParent = Weak();
	Add(index, spElement);
}

int SgaPackage::UnloadAllElementData() {
	int iUnloadedElementCount = 0;

	m_ElementMap.ForEachValue([&iUnloadedElementCount](const SgaElementPtr& element) {
		if (element->Unload()) {
			++iUnloadedElementCount;
		}
	});

	return iUnloadedElementCount;
}

bool SgaPackage::IsElementLoaded(const int index) const {
	return m_ElementMap.Exist(index);
}

int SgaPackage::GetElementIndex(const JCore::String& elementName) {
	bool bExist = m_ElementNameToIndex.Exist(elementName);
	DebugAssertMsg(bExist, "해당 엘리먼트 이름에 해당하는 인덱스가 없습니다.");
	return m_ElementNameToIndex[elementName];
}

bool SgaPackage::HasElementIndex(const JCore::String& elementName) const {
	bool bExist = m_ElementNameToIndex.Exist(elementName);
	return bExist;
}

SgaElement& SgaPackage::operator[](const int idx) {
	DebugAssert(m_ElementMap.Exist(idx));
	return m_ElementMap[idx].GetRef();
}

String SgaPackage::ToString() const {
	return StringUtil::Format("Sga(%s)", m_szPath.Source());
}
