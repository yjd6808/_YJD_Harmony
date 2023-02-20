/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:32:14 AM
 * =====================
 *
 */




 /*
  * 작성자: 윤정도
  * 생성일: 1/20/2023 1:57:14 PM
  * =====================
  *
  */

#include "Core.h"
#include "CommonInfoLoader.h"

#include <SteinsGate/Common/SGJson.h>

USING_NS_JS;
USING_NS_JC;

bool CommonInfoLoader::load() {
	Json::Value root;

	SGString szCommonFileName = ConfigFileType::FileName[getConfigFileType()];
	SGString szExeDir = SGPath::Combine(Env::CurrentDirectory(), szCommonFileName);

	if (!loadJson(szExeDir, root)) {
		return false;
	}

	try {
		Json::Value commonListRoot = root["common"];

		for (int i = 0; i < commonListRoot.size(); ++i) {
			Value& commontRoot = commonListRoot[i];
			CommonInfo* pCommonInfo = dbg_new CommonInfo;
			readCommonInfo(commontRoot, pCommonInfo);
			addData(pCommonInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}


void CommonInfoLoader::readCommonInfo(Json::Value& commonRoot, CommonInfo* commonInfo) {
	commonInfo->Code = commonRoot["code"].asInt();
	commonInfo->ConfigPath = SGJson::getString(commonRoot["config_dir"]);
	commonInfo->DataPath = SGJson::getString(commonRoot["data_dir"]);
}

