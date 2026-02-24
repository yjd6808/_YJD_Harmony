/*
 * 작성자: 윤정도
 * 내가 보기좋은 데이터 타입
 */

#pragma once

using _s64		= long long;
using _u64		= unsigned long long;
using object_id = _u64;
using _s32		= int;
using _s32		= int;
using _u32		= unsigned int;
using _s32l		= long;
using _u32l		= unsigned long;
using _s16		= short;
using _u16		= unsigned short;
using _s8		= char;
using _u8		= unsigned char;
using _s8		= char;
using _f32		= float;
using _f64		= double;
using _f64l		= long double;
using _u8		= unsigned char;
using _s16c		= wchar_t;

#ifdef _WIN64
using _ptr = _u64;
using _sz = unsigned long long;
#else
using _ptr = _u32;
using _sz = unsigned int;
#endif

using _whandle = void*;
using _wmodule = void*;
using _iohandle = void*;

