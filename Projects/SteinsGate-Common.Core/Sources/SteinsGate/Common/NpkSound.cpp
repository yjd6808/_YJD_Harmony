/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:25:59 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/NpkSound.h>
#include <SteinsGate/Common/NpkPackage.h>

using namespace JCore;

NpkSound::~NpkSound() {}

bool NpkSound::Load(bool loadIndexOnly) {
	if (loadIndexOnly) return true;
	if (m_spParent.Exist() == false) {
		return false;
	}

	m_spData = MakeShared<Byte[]>(m_iIndexLength);
	m_spParent->StreamRef().Seek(m_iIndexOffset);
	m_spParent->StreamRef().Read(m_spData.GetPtr(), 0, m_spData.Length());
	return true;
}

void NpkSound::Unload() {
	m_spData = nullptr;
}

NpkSoundPtr NpkSound::Create(const Header& header) {
	return MakeShared<NpkSound>(header);
}
