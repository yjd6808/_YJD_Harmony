/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:24:37 PM
 * =====================
 *
 */


#include "Core.h"
#include "ConfigFileLoaderAbstract.h"

ConfigFileLoaderAbstract::~ConfigFileLoaderAbstract() {
	m_hConfigDataAbstract.Values().Extension().ForEach([](ConfigDataAbstract* data) {
		delete data;
	});
}

bool ConfigFileLoaderAbstract::loadJson(Json::Value& root) {
	SGString path = JCore::Path::Combine(ConfigDirectoryPath_v, getConfigFileName());
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", getConfigFileName());
	try {
		reader >> root;
	} catch (std::exception& ex) {
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", getConfigFileName(), ex.what());
		return false;
	}
	return true;
}

void ConfigFileLoaderAbstract::addData(ConfigDataAbstract* data) {
	if (m_hConfigDataAbstract.Insert(data->Code, data) == false) {
		DebugAssertMsg(false, "%s 파일에서 이미 %d번 데이터를 읽은 후 입력하였습니다.", getConfigFileName(), data->Code);
	}
}

ConfigDataAbstract* ConfigFileLoaderAbstract::getData(int code) {
	DebugAssertMsg(m_hConfigDataAbstract.Exist(code), "%s 파일에서 읽은 데이터중 %d의 코드에 해당하는 데이터는 존재하지 않습니다.", getConfigFileName(), code);
	return m_hConfigDataAbstract[code];
}
