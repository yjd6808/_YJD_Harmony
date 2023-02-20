/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:10:38 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>


class ConfigFileLoaderAbstract
{
public:
	virtual ~ConfigFileLoaderAbstract();

	virtual bool loadJson(Out_ Json::Value& root);
	virtual bool loadJson(const char* fileName, Out_ Json::Value& root);
	virtual bool loadJson(const SGString& fileName, Out_ Json::Value& root);
	virtual void loadJsonThrow(const char* fileName, Out_ Json::Value& root);
	virtual void loadJsonThrow(const SGString& fileName, Out_ Json::Value& root);
	virtual bool load() = 0;
	
	virtual ConfigFileType_t getConfigFileType() = 0;
	const char* getConfigFileName() { return ConfigFileType::FileName[getConfigFileType()]; }

	void addData(ConfigDataAbstract* data);
	ConfigDataAbstract* getData(int code);



protected:
	SGHashMap<int, ConfigDataAbstract*> m_hConfigDataAbstract;
};


