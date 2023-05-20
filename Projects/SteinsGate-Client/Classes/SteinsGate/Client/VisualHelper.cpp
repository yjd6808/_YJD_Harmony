/*
 * 작성자: 윤정도
 * 생성일: 2/20/2023 6:18:47 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "VisualHelper.h"
#include "GameCoreHeader.h"



USING_NS_JS;
USING_NS_JC;
USING_NS_CC;


int VisualHelper::getVisualData(JCORE_OUT VisualData& visualData, int code) {
	ItemVisualInfo* pVisualInfo = CoreDataManager_v->getVisualInfo(code);
	CharType_t eCharType = pVisualInfo->getCharType();
	ItemType_t eItemType = pVisualInfo->getItemType();
	VisualType_t eVisualType = pVisualInfo->getVisualType();
	ImagePack* pPack = nullptr;

	char* ImgPrefix = nullptr;

	switch (eItemType) {
	case ItemType::Avatar:
	{
		AvatarType_t eAvatarType = pVisualInfo->getAvatarType();
		pPack = CorePackManager_v->getAvatarPack(eCharType, eAvatarType);
		ImgPrefix = (char*)AvatarType::ImgPrefix[eAvatarType];
		break;
	}
	case ItemType::Weapon:
	{
		WeaponType_t eWeaponType = pVisualInfo->getWeaponType();
		pPack = CorePackManager_v->getWeaponPack(eWeaponType);
		ImgPrefix = (char*)WeaponType::ImgPrefix[eWeaponType];
		break;
	}
	default:
		DebugAssertMsg(false, "해당 아이템 코드는 비주얼 아이템 코드가 아닙니다.");
		return InvalidValue_v;
	}

	DebugAssertMsg(pVisualInfo->Shape.Length() > 1, "해당 비주얼 아이템에 쉐이프가 없습니다.");

	int iZOrder = VisualType::ZOrder[eVisualType];
	int iSgaIndex = pPack->getPackIndex();
	int iShapeCount = pVisualInfo->ShapeAlpha.Length();	// 쉐이프 수

	// 알파가 없는 경우 그대로 씀
	if (iShapeCount == 0) {
		int iImgIndex = pPack->getImgIndex(
			StringUtil::Format("%s%s.img",
				ImgPrefix,
				pVisualInfo->Shape.Source()
			)
		);

		visualData[0].ZOrder = iZOrder;
		visualData[0].SgaIndex = iSgaIndex;
		visualData[0].ImgIndex = iImgIndex;
		return 1;
	}

	

	for (int i = 0; i < iShapeCount; ++i) {
		char alpha = pVisualInfo->ShapeAlpha[i];

		int iImgIndex = pPack->getImgIndex(
			StringUtil::Format("%s%s%c.img",
				ImgPrefix,
				pVisualInfo->Shape.Source(),
				alpha
			)
		);

		visualData[i].ZOrder = iZOrder--;
		visualData[i].SgaIndex = iSgaIndex;
		visualData[i].ImgIndex = iImgIndex;
	}

	return iShapeCount;
}
