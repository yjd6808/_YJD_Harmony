/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:04:30 AM
 * =====================
 *
 */

#pragma once

#if defined(STATIC_LIB_SGE)
	#define SGE_DLL
#else
	#define SGE_DLL
	// 지원 안함.
	//#if defined(SGE_DLL_EXPORT)
	//	#define SGE_DLL     __declspec(dllexport)
	//#else         /* use a DLL library */
	//	#define SGE_DLL     __declspec(dllimport)
	//#endif
#endif
