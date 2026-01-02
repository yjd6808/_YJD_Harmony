/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#pragma once

#include "sg/_API/sgapiBase.h"

class sgapiClient : public sgapiBase
{
public:
	virtual bool sgapi_IsClient() override { return true; }
};

