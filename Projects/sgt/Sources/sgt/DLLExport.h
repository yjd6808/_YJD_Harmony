/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:04:30 AM
 * =====================
 *
 */

#pragma once

#if defined(STATIC_LIB_STEINSGATE_TEST)
	#define SGT_DLL
#else
	#if defined(SGT_DLL_EXPORT)
		#define SGT_DLL     __declspec(dllexport)
	#else         /* use a DLL library */
		#define SGT_DLL     __declspec(dllimport)
	#endif
#endif
