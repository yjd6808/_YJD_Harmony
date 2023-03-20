/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:42:12 PM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/SgaImage.h>
#include <SteinsGate/Common/SgaSprite.h>
#include <SteinsGate/Common/SgaElementInitializer.h>

#include "SgaPackage.h"

USING_NS_JC;
USING_NS_JS;

SgaImage::SgaImage(const Header& header, int size)
	: SgaElement(SgaElement::eImage, header)
	, m_Sprites(size, nullptr)
	, m_iWaitForLoadingDataCount(size) {}

SgaImage::~SgaImage() {
}

SgaImagePtr SgaImage::Create(const Header& header, int size) {
	return MakeShared<SgaImage>(header, size);
}

SgaSpriteAbstract& SgaImage::operator[](const int idx) const {
	DebugAssert(IsValidIndex(idx));
	return m_Sprites[idx].GetRef();
}

bool SgaImage::IsValidIndex(int index) const {
	return index >= 0 && index < m_Sprites.Size();
}

bool SgaImage::Load(bool indexOnly) {
	if (!m_spParent.Exist())
		return false;

	SgaElementInitializer::InitializeElement(
		Shared(), 
		m_spParent->StreamRef(),
		indexOnly
	);
	return true;
}

bool SgaImage::Unload() {

	bool bHasUnloadedData = false;

	for (int i = 0; i < m_Sprites.Size(); ++i) {
		if (m_Sprites[i].GetRef().Unload()) {
			bHasUnloadedData = true;
		}
	}

	return bHasUnloadedData;
}

SgaSpriteAbstractPtr SgaImage::GetAt(const int idx) const {
	DebugAssert(IsValidIndex(idx));
	return m_Sprites[idx];
}

SgaSpriteAbstractPtr SgaImage::GetAtUnsafe(const int idx) const {
	if (!IsValidIndex(idx))
		return nullptr;

	return m_Sprites[idx];
}

SgaSpriteAbstract& SgaImage::GetAtRef(const int idx) const {
	DebugAssert(IsValidIndex(idx));
	return m_Sprites[idx].GetRef();
}

void SgaImage::Set(int idx, const SgaSpriteAbstractPtr& sprite) const {
	DebugAssertMsg(m_Sprites[idx] == nullptr, "이미 초기화된 스프라이트 인덱스에 넣을려고 하고 있습니다.");
	m_Sprites[idx] = sprite;
}

void SgaImage::Remove(const SgaSpriteAbstractPtr& sprite) {
	for (int i = 0; i < m_Sprites.Size(); i++) {
		if (m_Sprites[i] == sprite) {
			m_Sprites.RemoveAt(i--);
		}
	}
}

void SgaImage::RemoveAt(const int idx) {
	m_Sprites.RemoveAt(idx);
}

void SgaImage::Clear() {
	m_Sprites.Clear();
}


