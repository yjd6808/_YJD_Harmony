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

	bool TryGetTextRaw(const char* _id, OUT char** _pText);
	bool TryGetText(const char* _id, OUT SGString** _pText);
	bool TryGetText(const SGString& _id, OUT SGString** _pText);

	static bool ReadClientTextInfo(Json::Value& _clientTextRoot, OUT SGString& _id, OUT SGString& _text);

private:
	SGHashMap<SGString, SGString> textMap_;
};
