/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#pragma once

#include "sg/_API/sgapiBase.h"

class SG_DLL sgapiServerBase : public sgapiBase
{
public:
	virtual bool sgapi_IsClient() override { return false; }
	virtual bool sgapi_IsServer() override { return true; }

	virtual bool sgapi_IsInterServerClient() override;

	virtual int	sgapi_GetServerProcessType() { return serverProcessType_; } // ServerProcessType::None

protected:
	virtual void Init() override;

private:
	int		serverProcessType_ = 0;
	bool	isInterServerClient_ = false;
};

