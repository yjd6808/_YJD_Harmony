/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <sg/CharBaseInfoLoader.h>
#include <sgcl/CharInfo.h>

struct CharInfoLoader : CharBaseInfoLoader
{
public:
	CharInfoLoader(DescLoaderMgr* _pManager);

	virtual std::initializer_list<ConfigFileType_t> GetDependencies() override
	{
		return { ConfigFileType::Item, };
	}

	bool Load() override;
	static void ReadDefaultVisualInfo(Json::Value& _charRoot, OUT CharInfo* _pCharInfo);
};
