/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:36:58 AM
 * =====================
 *
 */

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>

#include <JCore/FileSystem/FileStream.h>

using namespace JCore;

NpkPackagePtr NpkPackage::Create(const JCore::StreamPtr& readOnlyStream, const String& path, int capacity) {
	return MakeShared<NpkPackage>(readOnlyStream, path, capacity);
}

void NpkPackage::Remove(const NpkElementPtr& element) {
	for (int i = 0; i < m_Elements.Size(); i++) {
		if (m_Elements[i] == element) {
			m_Elements.RemoveAt(i--);
		}
	}
}
