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
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGJson.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JS;
USING_NS_JC;

#define JsonFileName "char_animation.json"

bool SGCharAnimationInfoLoader::LoadCharAnimationInfo(
	SGHashMap<int, SGAnimationInfo> (&charAnimationInfoMap)[CharType::Max],
	SGVector<SGAnimationInfo*>(&charAnimationList)[CharType::Max])
{
	SGGlobal* pGlobal = SGGlobal::get();
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGString path = JCore::Path::Combine(ConfigDirectoryPath_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Json::Value root;
	try {
		reader >> root;

		for (int charCode = 0; charCode < CharType::Max; ++charCode) {

			Value& animationListRoot = root[CharType::Name[charCode]];
			for (int i = 0; i < animationListRoot.size(); ++i) {
				Value& animationRoot = animationListRoot[i];
				SGAnimationInfo animationInfo(animationRoot["frames"].size());
				SGJson::parseAnimationInfo(animationRoot, animationInfo);
				charAnimationInfoMap[charCode].Insert(animationInfo.Code, Move(animationInfo));

				// 값타입 해쉬맵이므로 여기서 이렇게 작성하면 안됨.
				// 해쉬맵에 애니메이션 정보 모두 삽입이 끝나고 넣어줘야함.
				// 1. 중간에 해쉬맵 확장이 발생할 수 있다. 따라서 애니메이션 데이터가 깨
				// 2. 중간에 해쉬맵 버킷 노드의 확장이 밣생할 수 있다. 따라서 애니메이션 데이터가 깨짐
				// charAnimationList[charCode].PushBack(&charAnimationInfoMap[iAnimCode]);
			}

			charAnimationInfoMap[charCode].Values().Extension().ForEach([charCode, &charAnimationList](SGAnimationInfo& info) {
				charAnimationList[charCode].PushBack(&info);
			});
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", JsonFileName, ex.what());
		return false;
	}

	return true;
}
