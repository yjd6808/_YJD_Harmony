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
public:
	ClientTextInfoLoader(DataManagerAbstract* manager);
	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::ClientText; }

	char* getTextRaw(const char* szId);
	SGString& getText(const char* szId);

	template <Int32U Size>
	SGString& getText(const char (&szId)[Size]) {
		SGString* pFind = m_TextMap.Find(szId);
		if (pFind == nullptr) {
			return m_Dummy;
		}
		return *pFind;
	}

	SGString& getText(const SGString& szId);

	static bool readClientTextInfo(Json::Value& clientTextRoot, JCORE_OUT SGString& szId, JCORE_OUT SGString& szText);

private:
	SGString m_Dummy{ 0 };
	SGHashMap<SGString, SGString> m_TextMap;
};


