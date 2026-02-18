/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#pragma once

class SG_DLL sgapiBase
{
public:
	// ----------------------------------------------------------------
	// 공통

	virtual bool sgapi_IsClient() { return false; }
	virtual bool sgapi_IsServer() { return false; }

	// ----------------------------------------------------------------
	// 서버
	virtual bool		sgapi_IsInterServerClient() { return false; }
	virtual int			sgapi_GetServerProcessType() { return 0; } // ServerProcessType::None
public:
	virtual ~sgapiBase() = default;
	virtual void Init();

	static void Init(sgapiBase* _api);
	static sgapiBase* Get();
	static void Free();
private:
	inline static sgapiBase* API;
	inline static bool isInitialized_ = false;
	inline static bool isDeleted_ = false;
};

#define g_cAPI				(*sgapiBase::Get())

