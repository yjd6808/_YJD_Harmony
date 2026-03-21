/*
 *	작성자 : 윤정도
 *	PCH 파일입니다.
 */

#pragma once

#pragma comment(lib, "winmm.lib")

#pragma warning(push, 0)

#pragma warning (disable : 4267)		// conversion from 'size_t' to 'type', possible loss of data
#pragma warning (disable : 26439)		// This kind of function may not throw.Declare it 'noexcept' (f.6)
#pragma warning (disable : 4018)		// '<': signed / unsigned mismatch

// Windows.h 헤더에 Winsock.h 헤더가 포함되는 것을 막는다. 그 외에 헤더들이 많이 추가되는 것을 방지 
// https://social.msdn.microsoft.com/Forums/vstudio/en-US/6177f9bc-db75-4317-9904-02ef55b527d0/linking-problem-with-winsock2?forum=vcgeneral
#define WIN32_LEAN_AND_MEAN	

#include <iostream>
#include <thread>

#include <Windows.h>
#include <winnt.h>
#include <exception>
#include <random>
#include <cstdlib>
#include <cstddef>

#include "jc/DLLExport.h"
#include "jc/Debug/MemoryLeakDetector.h"
#include "jc/Debug/New.h"

#include "jc/Encoding/CodePage.h"
#include "jc/Utils/Console.h"
#include "jc/Logger/LoggerDefine.h"
#include "jc/Define.h"
#include "jc/Macro.h"
#include "jc/Assert.h"
#include "jc/Math.h"
#include "jc/Bit.h"

#include "jc/Primitives/StringView.h"

#include "jc/Pool/ObjectPool.h"

#pragma warning(pop)
