/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:04:30 AM
 * =====================
 *
 */

#pragma once

#if defined(STATIC_LIB_JDB)
	#define JDB_DLL
#else
	#define JDB_DLL
	// 지원 안함.
	//#if defined(JDB_DLL_EXPORT)
	//	#define JDB_DLL     __declspec(dllexport)
	//#else         /* use a DLL library */
	//	#define JDB_DLL     __declspec(dllimport)
	//#endif
#endif
