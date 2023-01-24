/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:36:58 AM
 * =====================
 *
 */

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Common/Core/Npk/NpkPackage.h>

#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Path.h>

using namespace JCore;

NpkPackage::~NpkPackage() {
	printf("%s 패키지 소멸\n", JCore::Path::FileName(this->GetPath()).Source());
}

NpkPackagePtr NpkPackage::Create(const JCore::StreamPtr& readOnlyStream, const String& path, int capacity) {
	return MakeShared<NpkPackage>(readOnlyStream, path, capacity);
}

void NpkPackage::Add(const int idx, const NpkElementPtr& element) {
	m_ElementMap.Insert(idx, element);
}

NpkElementPtr NpkPackage::Get(int idx) {
	DebugAssertMessage(m_ElementMap.Exist(idx), "엘리먼트가 존재하지 않습니다.");
	return m_ElementMap[idx];
}

void NpkPackage::LoadElement(const int index, bool indexOnly) {
	DebugAssertMessage(index >= 0 && index < m_ElementHeaders.Size(), "올바르지 않은 Element 인덱스입니다.");
	NpkElementPtr spElement = NpkLoader::ReadElement(m_spStream.GetRef(), m_ElementHeaders[index], m_ElementHeaders[index].NextOffset, indexOnly);
	DebugAssertMessage(spElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
	spElement->m_spParent = Weak();
	Add(index, spElement);
}

bool NpkPackage::IsElementLoaded(const int index) {
	return m_ElementMap.Exist(index);
}

int NpkPackage::GetElementIndex(const JCore::String& elementName) {
	bool bExist = m_ElementNameToIndex.Exist(elementName);
	DebugAssertMessage(bExist, "해당 엘리먼트 이름에 해당하는 인덱스가 없습니다.");
	return m_ElementNameToIndex[elementName];
}

bool NpkPackage::HasElementIndex(const JCore::String& elementName) {
	bool bExist = m_ElementNameToIndex.Exist(elementName);
	return bExist;
}
