/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:57:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGCharAnimationInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGAnimationInfo.h>
#include <SteinsGate/Client/SGJsonEx.h>


USING_NS_JS;
USING_NS_JC;

SGCharAnimationInfoLoader::SGCharAnimationInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool SGCharAnimationInfoLoader::load()
{
	Json::Value root;

	if (!loadJson(root))
		return false;

	try {

		for (int charCode = 0; charCode < CharType::Max; ++charCode) {

			Value& animationListRoot = root[CharType::Name[charCode]];
			for (int i = 0; i < animationListRoot.size(); ++i) {
				Value& animationRoot = animationListRoot[i];
				SGAnimationInfo* pInfo = dbg_new SGAnimationInfo(animationRoot["frames"].size());
				SGJsonEx::parseAnimationInfo(animationRoot, *pInfo);
				m_CharAnimationList[charCode].PushBack(pInfo);
				addData(pInfo);
			}
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

SGVector<SGAnimationInfo*>& SGCharAnimationInfoLoader::getAnimationList(CharType_t charCode) {
	DebugAssertMsg(charCode >= CharType::Begin && charCode <= CharType::End, "해당 캐릭터 타입은 존재하지 않습니다.");
	return m_CharAnimationList[charCode];
}
