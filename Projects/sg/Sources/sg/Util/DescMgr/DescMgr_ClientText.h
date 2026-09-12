/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */

#pragma once

#include "sg/Util/DescLoaderAbstract.h"

struct ClientTextInfoLoader : DescLoaderAbstract
{
	inline static jc::String DummyText{ _T("메시지가 엄떠용 ㅠㅠ") };

	explicit ClientTextInfoLoader();
	bool Load() override;
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::ClientText; }

	bool TryGetText(const jc::String& _id, OUT jc::String** _pText);
#ifndef _UNICODE
	bool TryGetTextRaw(const char* _id, OUT char** _pText);
	bool TryGetText(const char* _id, OUT jc::String** _pText);
#endif

	static bool ReadClientTextInfo(Json::Value& _clientTextRoot, OUT jc::String& _id, OUT jc::String& _text);

private:
	jc::HashMap<jc::String, jc::String> textMap_;
};
