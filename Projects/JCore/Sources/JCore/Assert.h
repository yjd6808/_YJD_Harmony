#pragma once

#include <cassert>

#ifndef DebugAssert
    #if DebugMode
	    #define DebugAssertMessage(exp, msg)	assert((exp) && (msg))
        #define DebugAssert(exp)                assert((exp))
    #else
        #define DebugAssertMessage(exp, msg)	(0)
        #define DebugAssert(exp)                (0)
    #endif
#endif