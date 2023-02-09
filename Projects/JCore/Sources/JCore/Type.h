/*
 * 작성자: 윤정도
 * 내가 보기좋은 데이터 타입
 */

#pragma once

using Int64		= long long;
using Int64U	= unsigned long long;
using Int		= int;
using Int32		= int;
using Int32U	= unsigned int;
using Int32L	= long;
using Int32UL	= unsigned long;
using Int16		= short;
using Int16U	= unsigned short;
using Int8		= char;
using Int8U		= unsigned char;
using Char		= char;
using Double	= double;
using LDouble	= long double;
using Byte		= unsigned char;
using WideChar	= wchar_t;

#ifdef _WIN64
using IntPtr = Int64;
using Size_t = unsigned long long;
#else
using IntPtr = Int32U;
using Size_t = unsigned int;
#endif

using WinHandle = void*;
using WinModule = void*;
using IoHandle = void*;

