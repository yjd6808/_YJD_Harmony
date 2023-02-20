/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:25:59 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/SgaSound.h>
#include <SteinsGate/Common/SgaPackage.h>

USING_NS_JC;

SgaSound::~SgaSound() {}

bool SgaSound::Load(bool loadIndexOnly) {
	if (loadIndexOnly) return true;
	if (m_spParent.Exist() == false) {
		return false;
	}

	m_spData = MakeShared<Byte[]>(m_iIndexLength);
	m_spParent->StreamRef().Seek(m_iIndexOffset);
	m_spParent->StreamRef().Read(m_spData.GetPtr(), 0, m_spData.Length());
	return true;
}

bool SgaSound::Unload() {
	if (!m_spData.Exist())
		return false;

	m_spData = nullptr;
	return true;
}

SgaSoundPtr SgaSound::Create(const Header& header) {
	return MakeShared<SgaSound>(header);
}
