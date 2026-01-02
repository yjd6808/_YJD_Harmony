/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 8:13:09 AM
 * =====================
 *
 */

#pragma once

#include <sg/Config.h>

struct RuntimeConfigCommon : RuntimeConfig
{
	RuntimeConfigCommon();
	~RuntimeConfigCommon() override;

	void ReadCommon(Json::Value& _jsonRoot);
	void WriteCommon(Json::Value& _jsonRoot);
};
