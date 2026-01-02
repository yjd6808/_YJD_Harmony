/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:04:30 AM
 * =====================
 *
 */

#pragma once

#if defined(STATIC_LIB_STEINSGATE_COCOS)
    #define SGCC_DLL
#else
	#if defined(SGCC_DLL_EXPORT)
		#define SGCC_DLL     __declspec(dllexport)
	#else         /* use a DLL library */
		#define SGCC_DLL     __declspec(dllimport)
	#endif
#endif
