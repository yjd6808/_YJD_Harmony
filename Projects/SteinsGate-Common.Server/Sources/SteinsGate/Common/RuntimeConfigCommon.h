/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 8:13:09 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/RuntimeConfigBase.h>

struct RuntimeConfigCommon : RuntimeConfigBase
{
	RuntimeConfigCommon();
	~RuntimeConfigCommon() override;

	void ReadCommon(Json::Value& root);
	void WriteCommon(Json::Value& root);
};

