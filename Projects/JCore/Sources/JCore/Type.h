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
using WChar		= wchar_t;

using PInt64	= long long*;
using PInt64U	= unsigned long long*;
using PInt		= int*;
using PInt32	= int*;
using PInt32U	= unsigned int*;
using PInt32L	= long*;
using PInt32UL	= unsigned long*;
using PInt16	= short*;
using PInt16U	= unsigned short*;
using PInt8		= char*;
using PInt8U	= unsigned char*;
using PChar		= char*;
using PByte		= unsigned char*;
using PWChar	= wchar_t*;

#ifdef _WIN64
using IntPtr = Int64;
#else
using IntPtr = Int32U;
#endif

using WinHandle = void*;

