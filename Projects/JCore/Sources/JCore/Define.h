#if !defined(__JCORE_DEFINE_H__)

    #define __JCORE_DEFINE_H__
    #define DeleteSafe(x)		\
    do {						\
	    if (x) {				\
		    delete (x);			\
        (x) = nullptr;			\
	    }						\
    } while (0)

    #define DeleteArraySafe(x)	\
    do {						\
	    if (x) {				\
		    delete[] (x);		\
        (x) = nullptr;		  	\
	    }						\
    } while (0)		

    #define JCoreStdCall        __stdcall
    #define JCoreCdecl          __cdecl
    #define JCoreForceInline    __forceinline
    #define JCoreInfinite       0xffffffff

    #define In_
    #define Out_
	#define OutOpt_
    #define InOut_
    #define InOpt_

    

#endif