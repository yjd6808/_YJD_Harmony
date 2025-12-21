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

//////////////////////////////////////////////////////////////////////////////////////////
int VisualHelper::GetVisualData(JCORE_OUT VisualData& _visualData, int _code)
{
	ItemVisualInfo* pVisualInfo = Core::DataManager->getVisualInfo(_code);
	CharType_t charType = pVisualInfo->GetCharType();
	ItemType_t itemType = pVisualInfo->GetItemType();
	VisualType_t visualType = pVisualInfo->GetVisualType();
	ImagePack* pPack = nullptr;

	char* pImgPrefix = nullptr;

	switch (itemType)
	{
	case ItemType::Avatar:
		{
			AvatarType_t avatarType = pVisualInfo->GetAvatarType();
			pPack = Core::Contents.PackManager->GetAvatarPack(charType, avatarType);
			pImgPrefix = (char*)AvatarType::ImgPrefix[avatarType];
			break;
		}
	case ItemType::Weapon:
		{
			WeaponType_t weaponType = pVisualInfo->GetWeaponType();
			pPack = Core::Contents.PackManager->GetWeaponPack(weaponType);
			pImgPrefix = (char*)WeaponType::ImgPrefix[weaponType];
			break;
		}
	default:
		DebugAssertMsg(false, "해당 아이템 코드는 비주얼 아이템 코드가 아닙니다.");
		return InvalidValue_v;
	}

	DebugAssertMsg(pVisualInfo->shape_.Length() > 1, "해당 비주얼 아이템에 쉐이프가 없습니다.");

	int zOrder = VisualType::ZOrder[visualType];
	int sgaIndex = pPack->GetPackIndex();
	int shapeCount = pVisualInfo->shapeAlpha_.Length(); // 쉐이프 수

	// 알파가 없는 경우 그대로 씀
	if (shapeCount == 0)
	{
		int imgIndex = pPack->GetImgIndex(
			StringUtil::Format("%s%s.img",
			                   pImgPrefix,
			                   pVisualInfo->shape_.Source()
			)
		);

		_visualData[0].zOrder_ = zOrder;
		_visualData[0].sgaIndex_ = sgaIndex;
		_visualData[0].imgIndex_ = imgIndex;
		return 1;
	}

	for (int shapeIndex = 0; shapeIndex < shapeCount; ++shapeIndex)
	{
		char alpha = pVisualInfo->shapeAlpha_[shapeIndex];

		int imgIndex = pPack->GetImgIndex(
			StringUtil::Format("%s%s%c.img",
			                   pImgPrefix,
			                   pVisualInfo->shape_.Source(),
			                   alpha
			)
		);

		_visualData[shapeIndex].zOrder_ = zOrder--;
		_visualData[shapeIndex].sgaIndex_ = sgaIndex;
		_visualData[shapeIndex].imgIndex_ = imgIndex;
	}

	return shapeCount;
}
