/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "CharInfoLoader.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/CharInfo.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>
#include <SteinsGate/Client/VisualHelper.h>

USING_NS_CC;
USING_NS_JS;
USING_NS_JC;

CharInfoLoader::CharInfoLoader(DataManagerAbstract* manager)
	: CharBaseInfoLoader(manager)
{}

bool CharInfoLoader::load() {
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {
		Json::Value gunnerRoot = root["gunner"];
		// 다른 캐릭이 만약 추가되면 코드 변경 필요
		GunnerInfo* pGunnerInfo = dbg_new GunnerInfo();
		readCharBaseInfo(gunnerRoot, pGunnerInfo);
		readGunnerInfo(gunnerRoot, pGunnerInfo);
		readDefaultVisualInfo(gunnerRoot, pGunnerInfo);
		addData(pGunnerInfo);
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;

}

void CharInfoLoader::readDefaultVisualInfo(Json::Value& charRoot, JCORE_OUT CharInfo* charInfo) {
	ImagePackManager* pPackManager = ImagePackManager::Get();
	Global* pGlobal = Global::Get();
	Json::Value& defaultVisualInfoRoot = charRoot["default_visual_img"];

	auto eCharType = (CharType_t)charInfo->Code;

	for (int i = 0; i < VisualType::Max; ++i) {
		const char* pVisualTypeName = VisualType::Name[i];
		int iDefaultCode = defaultVisualInfoRoot.get(pVisualTypeName, InvalidValue_v).asInt();

		if (iDefaultCode == InvalidValue_v) {
			charInfo->HasVisual[i] = false;
			continue;
		}

		ItemCode code;
		if (VisualType::IsAvatar[i]) {
			code.initAvatarCode(eCharType, (AvatarType_t)i, iDefaultCode);
		} else if (VisualType::IsWeapon[i]) {
			code.initWeaponCode(eCharType, charInfo->DefaultWeaponType, iDefaultCode);
		} else {
			DebugAssertMsg(false, "무슨 타입이죠?");
			continue;
		}

		charInfo->VisualCount[i] = VisualHelper::getVisualData(charInfo->Visual[i], code.Code);
		charInfo->HasVisual[i] = true;
	}
	
}


