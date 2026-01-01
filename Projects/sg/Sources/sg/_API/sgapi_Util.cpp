/*
 * 작성자: 윤정도
 * 생성일: 1/1/2026 11:20:44 AM
 * =====================
 *
 */

#include "Core.h"
#include "sgapiBase.h"

//////////////////////////////////////////////////////////////////////////////////////////
bool sgapi_IsClient()
{
	return g_cAPI.sgapi_IsClient();
}