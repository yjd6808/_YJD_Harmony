/*
 *	작성자 : 윤정도
 *	PCH 파일입니다.
 */

#pragma once

#pragma warning (disable : 4267)		// conversion from 'size_t' to 'type', possible loss of data
#pragma warning (disable : 26439)		// This kind of function may not throw.Declare it 'noexcept' (f.6)
#pragma warning (disable : 4018)		// '<': signed / unsigned mismatch

#define WIN32_LEAN_AND_MEAN	

#include <iostream>
#include <thread>
#include <Windows.h>
#include <exception>
#include <random>
#include <JCore/Define.h>
#include <JCore/Assert.h>

