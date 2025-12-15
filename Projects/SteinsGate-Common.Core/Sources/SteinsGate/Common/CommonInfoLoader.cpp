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

#include <SteinsGate/Common/DataManagerAbstract.h>
#include <SteinsGate/Common/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
CommonInfoLoader::CommonInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CommonInfoLoader::Load()
{
	Json::Value root;

	SGString commonFileName = ConfigFileType::FileName[GetConfigFileType()];
	SGString exeDir = SGPath::Combine(Env::CurrentDirectory(), commonFileName);

	if (!JsonUtil::Load(exeDir, root))
	{
		return false;
	}

	try
	{
		Json::Value& commonListRoot = root["common"];

		for (int commonIndex = 0; commonIndex < commonListRoot.size(); ++commonIndex)
		{
			Value& commontRoot = commonListRoot[commonIndex];
			CommonInfo* pCommonInfo = dbg_new CommonInfo;
			ReadCommonInfo(commontRoot, pCommonInfo);
			AddData(pCommonInfo);
		}
	}
	catch (std::exception& _ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), _ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonInfoLoader::ReadCommonInfo(Json::Value& _commonRoot, CommonInfo* _pCommonInfo)
{
	_pCommonInfo->code_ = _commonRoot["code"].asInt();
	_pCommonInfo->configPath_ = JsonUtil::GetString(_commonRoot["config_dir"]);
	_pCommonInfo->dataPath_ = JsonUtil::GetString(_commonRoot["data_dir"]);
}
