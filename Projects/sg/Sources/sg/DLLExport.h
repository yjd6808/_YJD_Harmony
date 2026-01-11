/*
 * 작성자: 윤정도
 * 생성일: 1/11/2026 8:26:28 PM
 * =====================
 *
 */

#pragma once

#if defined(STATIC_LIB_STEINSGATE)
	#define SG_DLL
#else
	#define SG_DLL
	 // 아직 dll 지원안함.
	 //#if defined(SG_DLL_EXPORT)
	 //	#define SG_DLL     __declspec(dllexport)
	 //#else         /* use a DLL library */
	 //	#define SG_DLL     __declspec(dllimport)
	 //#endif
#endif
