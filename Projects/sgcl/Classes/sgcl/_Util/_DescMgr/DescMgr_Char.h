/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include "sg/_Util/_DescMgr/DescMgr_CharBase.h"
#include "sg/_Struct/SteinsGate_Char.h"

struct CharInfoLoader : CharBaseInfoLoader
{
public:
	CharInfoLoader();

	virtual std::initializer_list<ConfigFileType_t> GetDependencies() override
	{
		return { ConfigFileType::Item, };
	}

	bool Load() override;
	static void ReadDefaultVisualInfo(Json::Value& _charRoot, OUT CharInfo* _pCharInfo);
};
