#pragma once

#if defined(STATIC_LIB_JC)
	#define JC_DLL
#else
	#define JC_DLL
	 // 아직 dll 지원안함.
	 //#if defined(SG_DLL_EXPORT)
	 //	#define SG_DLL     __declspec(dllexport)
	 //#else         /* use a DLL library */
	 //	#define SG_DLL     __declspec(dllimport)
	 //#endif
#endif
