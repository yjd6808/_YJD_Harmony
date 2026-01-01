/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:28 PM
 * =====================
 *
 */

#pragma once

#include <sg/DescLoaderAbstract.h>
#include <sgcl/ActionInfo.h>

struct ActionInfoLoader : DescLoaderAbstract
{
public:
	ActionInfoLoader(DescLoaderMgr* _pManager);
	~ActionInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override;
	bool Load() override;

	static void ReadActionInfo(Json::Value& _actionRoot, OUT ActionInfo* _pActionInfo);
};
