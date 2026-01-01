/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#pragma once

class sgapiBase
{
public:
	virtual bool sgapi_IsClient() { return false; }

public:
	virtual ~sgapiBase() = default;
	static void Init(sgapiBase* _api);
	static sgapiBase* Get();
	static void Free();
private:
	inline static sgapiBase* API;
	inline static bool isInitialized_ = false;
	inline static bool isDeleted_ = false;
};

#define g_cAPI				(*sgapiBase::Get())

