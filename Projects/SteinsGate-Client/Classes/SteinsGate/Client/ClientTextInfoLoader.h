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

	explicit ClientTextInfoLoader(DataManagerAbstract* _pManager);
	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::ClientText; }

	bool tryGetTextRaw(const char* _id, JCORE_OUT char** _pText);
	bool tryGetText(const char* _id, JCORE_OUT SGString** _pText);
	bool tryGetText(const SGString& _id, JCORE_OUT SGString** _pText);

	static bool readClientTextInfo(Json::Value& _clientTextRoot, JCORE_OUT SGString& _id, JCORE_OUT SGString& _text);

private:
	SGHashMap<SGString, SGString> textMap_;
};
