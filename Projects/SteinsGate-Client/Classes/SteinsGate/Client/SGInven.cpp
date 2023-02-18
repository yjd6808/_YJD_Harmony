/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 10:32:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGInven.h"

#include <SteinsGate/Client/SGDataManager.h>

SGInven::SGInven()
	: m_hashInven{}
	, m_EquipedItemList{}
	, m_EquipedAvatarList{}
{}

SGInven* SGInven::get() {
	static SGInven* s_pInst;

	if (s_pInst == nullptr) {
		s_pInst = dbg_new SGInven;
	}

	return s_pInst;
}

void SGInven::getVisualInfo(Out_ VisualInfo& info, int defaultCharType) {

	SGCharInfo* pDefaultCharInfo = CoreDataManager_v->getCharInfo(defaultCharType);

	for (int i = 0; i < VisualType::Max; ++i) {
		info.NpkIndex[i] = pDefaultCharInfo->DefaultVisualNpkIndex[i];
		info.ImgIndex[i] = pDefaultCharInfo->DefaultVisualImgIndex[i];
	}

	for (int i = 0; i < VisualType::Max; ++i) {
		if (m_EquipedAvatarList[i] != nullptr) {

		} else {
			
		}
	}
}
