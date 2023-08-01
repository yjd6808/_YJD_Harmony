/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct ClientTextInfoLoader : ConfigFileLoaderAbstract
{
	inline static SGString DummyText{ "메시지가 엄떠용 ㅠㅠ" };

	ClientTextInfoLoader(DataManagerAbstract* manager);
	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::ClientText; }

	bool tryGetTextRaw(const char* id, JCORE_OUT char** text);
	bool tryGetText(const char* id, JCORE_OUT SGString** text);
	bool tryGetText(const SGString& id, JCORE_OUT SGString** text);

	static bool readClientTextInfo(Json::Value& clientTextRoot, JCORE_OUT SGString& szId, JCORE_OUT SGString& szText);

private:
	
	SGHashMap<SGString, SGString> m_TextMap;
};


