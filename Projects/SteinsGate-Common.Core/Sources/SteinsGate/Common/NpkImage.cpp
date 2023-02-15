/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:42:12 PM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/NpkImage.h>
#include <SteinsGate/Common/NpkSprite.h>
#include <SteinsGate/Common/NpkElementInitializer.h>

#include "NpkPackage.h"

using namespace JCore;

NpkImagePtr NpkImage::Create(const Header& header) {
	return MakeShared<NpkImage>(header);
}

bool NpkImage::Load(bool indexOnly) {
	if (!m_spParent.Exist())
		return false;

	NpkElementInitializer::InitializeElement(
		Shared(), 
		m_spParent->StreamRef(),
		indexOnly
	);
	m_bIndexLoaded = true;
	return true;
}

bool NpkImage::Unload() {

	bool bHasUnloadedData = false;

	for (int i = 0; i < m_Sprites.Size(); ++i) {
		if (m_Sprites[i].GetRef().Unload()) {
			bHasUnloadedData = true;
		}
	}

	return bHasUnloadedData;
}

NpkImage::~NpkImage() {
}

void NpkImage::Add(NpkSpriteAbstractPtr sprite) {
	m_Sprites.PushBack(sprite);
}

void NpkImage::Remove(NpkSpriteAbstractPtr sprite) {
	for (int i = 0; i < m_Sprites.Size(); i++) {
		if (m_Sprites[i] == sprite) {
			m_Sprites.RemoveAt(i--);
		}
	}
}

void NpkImage::RemoveAt(const int idx) {
	m_Sprites.RemoveAt(idx);
}

void NpkImage::Clear() {
	m_Sprites.Clear();
}
