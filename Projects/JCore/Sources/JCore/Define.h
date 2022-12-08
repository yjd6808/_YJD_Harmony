#if !defined(__JCORE_DEFINE_H__)

    #define __JCORE_DEFINE_H__
    #define DeleteSafe(x)		\
    do {						\
	    if (x) {				\
		    delete (x);			\
        (x) = nullptr;			\
	    }						\
    } while (0);

    #define DeleteArraySafe(x)	\
    do {						\
	    if (x) {				\
		    delete[] (x);		\
        (x) = nullptr;		  	\
	    }						\
    } while (0);		

    #define JCoreWinApi         __stdcall
    #define JCoreSystem         __cdecl
    #define JCoreForceInline    __forceinline

    #define In_
    #define Out_
    #define InOut_
    #define InOpt_

    

#endif