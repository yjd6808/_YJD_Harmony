/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:28 PM
 * =====================
 *
 */

#pragma once

#include "sg/Util/DescLoaderAbstract.h"
#include "sg/Struct/SteinsGate_Action.h"

struct ActionInfoLoader : DescLoaderAbstract
{
public:
	ActionInfoLoader();
	~ActionInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override;
	bool Load() override;

	static void ReadActionInfo(Json::Value& _actionRoot, OUT ActionInfo* _pActionInfo);
};
