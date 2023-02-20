/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGCharInfoLoader.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGCharInfo.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGJsonEx.h>
#include <SteinsGate/Client/VisualHelper.h>

USING_NS_CC;
USING_NS_JS;
USING_NS_JC;


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
		readDefaultVisualInfo(gunnerRoot, pGunnerInfo);
		addData(pGunnerInfo);
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;

}

void SGCharInfoLoader::readDefaultVisualInfo(Json::Value& charRoot, Out_ SGCharInfo* charInfo) {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGGlobal* pGlobal = SGGlobal::get();
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

		DebugAssertMsg(charInfo->VisualCount[i] > 0, "안들어오겠지?");
	}
	
}


