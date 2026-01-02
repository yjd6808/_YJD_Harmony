/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Core.h"
#include "DescMgr_Char.h"

#include <sgcl/JsonUtilEx.h>
#include <sgcl/VisualHelper.h>

USING_NS_CC;
USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
CharInfoLoader::CharInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CharInfoLoader::Load()
{
	LoadDepedencies();

	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Json::Value gunnerRoot = root["gunner"];
		// 다른 캐릭이 만약 추가되면 코드 변경 필요
		GunnerInfo* pGunnerInfo = dbg_new GunnerInfo();
		ReadCharBaseInfo(gunnerRoot, pGunnerInfo);
		ReadGunnerInfo(gunnerRoot, pGunnerInfo);
		ReadDefaultVisualInfo(gunnerRoot, pGunnerInfo);
		AddData(pGunnerInfo);
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CharInfoLoader::ReadDefaultVisualInfo(Json::Value& _charRoot, OUT CharInfo* _pCharInfo)
{
	Json::Value& defaultVisualInfoRoot = _charRoot["default_visual_img"];

	CharType_t charType = static_cast<CharType_t>(_pCharInfo->code_);

	for (int visualIndex = 0; visualIndex < VisualType::Max; ++visualIndex)
	{
		const char* pVisualTypeName = VisualType::Name[visualIndex];
		int defaultCode = defaultVisualInfoRoot.get(pVisualTypeName, InvalidValue_v).asInt();

		if (defaultCode == InvalidValue_v)
		{
			_pCharInfo->hasVisual_[visualIndex] = false;
			continue;
		}

		ItemCode itemCode;
		if (VisualType::IsAvatar[visualIndex])
		{
			itemCode.InitAvatarCode(charType, static_cast<AvatarType_t>(visualIndex), defaultCode);
		}
		else if (VisualType::IsWeapon[visualIndex])
		{
			itemCode.InitWeaponCode(charType, _pCharInfo->defaultWeaponType_, defaultCode);
		}
		else
		{
			jc_assert_msg(false, "무슨 타입이죠?");
			continue;
		}

		_pCharInfo->visualCount_[visualIndex] = VisualHelper::GetVisualData(_pCharInfo->visual_[visualIndex], itemCode.Code);
		_pCharInfo->hasVisual_[visualIndex] = true;
	}
}
