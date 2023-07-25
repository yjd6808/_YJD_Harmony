/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:24:37 PM
 * =====================
 *
 */


#include "Core.h"
#include "DataManagerAbstract.h"
#include "CommonCoreHeader.h"

ConfigFileLoaderAbstract::ConfigFileLoaderAbstract(DataManagerAbstract* manager)
	: m_pManager(manager)
{}

ConfigFileLoaderAbstract::~ConfigFileLoaderAbstract() {
	m_hConfigDataAbstract.ForEachValue([](ConfigDataAbstract* data) {
		delete data;
	});
}

bool ConfigFileLoaderAbstract::loadJson(JCORE_OUT Json::Value& root) {
	DebugAssertMsg(CoreCommonInfo_v != nullptr, "프로그램 실행 후 즉시 CoreCommon_v를 초기화해서 설정/데이터 파일 경로를 확보해주세요. 데이터 매니저 생성시 곧바로 실행파일 경로의 common.json이 로딩되므로 원하는 설정 코드로 세팅하면 됩니다.");
	SGString path = JCore::Path::Combine(CoreCommonInfo_v->ConfigPath, getConfigFileName());
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

bool ConfigFileLoaderAbstract::loadJson(const char* fileName, JCORE_OUT Json::Value& root) {
	std::ifstream reader(fileName, std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", fileName);
	try {
		reader >> root;
	} catch (std::exception& ex) {
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", fileName, ex.what());
		return false;
	}
	return true;
}

bool ConfigFileLoaderAbstract::loadJson(const SGString& fileName, JCORE_OUT Json::Value& root) {
	return loadJson(fileName.Source(), root);
}

void ConfigFileLoaderAbstract::loadJsonThrow(const char* fileName, Json::Value& root) {
	std::ifstream reader(fileName, std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", fileName);
	reader >> root;
}

void ConfigFileLoaderAbstract::loadJsonThrow(const SGString& fileName, Json::Value& root) {
	loadJsonThrow(fileName.Source(), root);
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

