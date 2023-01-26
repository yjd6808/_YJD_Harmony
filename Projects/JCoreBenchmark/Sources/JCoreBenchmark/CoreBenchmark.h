/*
 * 작성자: 윤정도
 * 생성일: 12/11/2022 3:19:00 PM
 * =====================
 * PCH 파일
 */


#pragma once


#include <JCore/Core.h>

#include <JCore/Encoding/CodePage.h>
#include <JCore/Utils/Console.h>
#include <JCore/Primitives/Atomic.h>

#pragma warning(push, 0)
#include <benchmark/benchmark.h>
#include <iostream>
#include <thread>
#include <cassert>
#pragma warning(pop)

using namespace JCore;
using namespace benchmark;
using namespace std;

using Console = NormalConsole;

#define ON						1
#define OFF						0


// ========================================================================================
// Research: 구조 고민에 앞서서 함수 사용 연습 및 성능 테스트를 위한 폴더
// ========================================================================================
#define VirtualAllocTest		OFF
#define AllocationTest			OFF

// ========================================================================================
// Container
// ========================================================================================

#define HashMapTest				OFF
#define HashMapMemoryPoolTest	ON