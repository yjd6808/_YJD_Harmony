/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#include "Core.h"
#include "sgapiBase.h"

//////////////////////////////////////////////////////////////////////////////////////////
void sgapiBase::Init(sgapiBase* _api)
{
	API = _api;
	isInitialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
sgapiBase* sgapiBase::Get()
{
	if (API == nullptr)
	{
		jc_assert(isInitialized_); // 초기화 되지 않았는데 사용한 경우 알려준다.
		jc_assert(!isDeleted_); // 삭제되었는데 사용한 경우 알려준다.

		static sgapiBase dummy;
		return &dummy;
	}

	return API;
}

//////////////////////////////////////////////////////////////////////////////////////////
void sgapiBase::Free()
{
	JC_DELETE_SAFE(API);
	isDeleted_ = true;
}
