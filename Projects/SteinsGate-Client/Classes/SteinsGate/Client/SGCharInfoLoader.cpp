/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGCharInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGCharInfo.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGJsonEx.h>

USING_NS_CC;
USING_NS_JS;


bool SGCharInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value gunnerRoot = root["gunner"];

		// 다른 캐릭이 만약 추가되면 코드 변경 필요
		SGGunnerInfo* pGunnerInfo = dbg_new SGGunnerInfo();
		readCharBaseInfo(gunnerRoot, pGunnerInfo);
		readGunnerInfo(gunnerRoot, pGunnerInfo);
		readCharInfo(gunnerRoot, pGunnerInfo);
		addData(pGunnerInfo);
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;

}

void SGCharInfoLoader::readCharInfo(Json::Value& gunnerRoot, Out_ SGGunnerInfo* gunnerInfo) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGGlobal* pGlobal = SGGlobal::get();
	Json::Value& defaultAvatarImgRoot = gunnerRoot["default_visual_img"];

	SGString defaultAvatarPartImgName[VisualType::Max];

	defaultAvatarPartImgName[VisualType::Skin] = SGJsonEx::getString(defaultAvatarImgRoot["skin"]);
	defaultAvatarPartImgName[VisualType::Shoes] = SGJsonEx::getString(defaultAvatarImgRoot["shoes"]);
	defaultAvatarPartImgName[VisualType::Pants] = SGJsonEx::getString(defaultAvatarImgRoot["pants"]);
	defaultAvatarPartImgName[VisualType::Neck] = SGJsonEx::getString(defaultAvatarImgRoot["neck"]);
	defaultAvatarPartImgName[VisualType::Hair] = SGJsonEx::getString(defaultAvatarImgRoot["hair"]);
	defaultAvatarPartImgName[VisualType::Face] = SGJsonEx::getString(defaultAvatarImgRoot["face"]);
	defaultAvatarPartImgName[VisualType::Coat] = SGJsonEx::getString(defaultAvatarImgRoot["coat"]);
	defaultAvatarPartImgName[VisualType::Cap] = SGJsonEx::getString(defaultAvatarImgRoot["cap"]);
	defaultAvatarPartImgName[VisualType::Belt] = SGJsonEx::getString(defaultAvatarImgRoot["belt"]);
	defaultAvatarPartImgName[VisualType::Weapon] = SGJsonEx::getString(defaultAvatarImgRoot["weapon"]);


	for (int iVisualType = AvatarType::Begin; iVisualType < AvatarType::Max; ++iVisualType) {
		const SGString& npkName = pGlobal->getAvatarNpkName(CharType::Gunner, iVisualType);
		SGImagePack* pImgPack = pPackManager->getPack(npkName);

		gunnerInfo->DefaultVisualNpkIndex[iVisualType] = pImgPack->getPackIndex();
		if (pImgPack->hasImgIndex(defaultAvatarPartImgName[iVisualType]))
			gunnerInfo->DefaultVisualImgIndex[iVisualType] = pImgPack->getImgIndex(defaultAvatarPartImgName[iVisualType]);
	}

	bool bDefaultWeaponImgFound = false;
	for (int iWeaponType = WeaponType::Begin; iWeaponType < WeaponType::Max; ++iWeaponType) {
		const SGString& npkName = pGlobal->getWeaponNpkName(CharType::Gunner, iWeaponType);
		SGImagePack* pImgPack = pPackManager->getPack(npkName);

		if (pImgPack->hasImgIndex(defaultAvatarPartImgName[VisualType::Weapon])) {
			gunnerInfo->DefaultVisualImgIndex[VisualType::Weapon] = pImgPack->getImgIndex(defaultAvatarPartImgName[VisualType::Weapon]);
			gunnerInfo->DefaultVisualNpkIndex[VisualType::Weapon] = pImgPack->getPackIndex();
			bDefaultWeaponImgFound = true;
		}
	}

	DebugAssertMsg(bDefaultWeaponImgFound, "기본 무기 이미지가 없습니다.");
}


