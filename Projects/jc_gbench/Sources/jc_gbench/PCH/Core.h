/*
 * 작성자: 윤정도
 * 생성일: 12/11/2022 3:19:00 PM
 * =====================
 * PCH 파일
 */


#pragma once

#include "jc/PCH/Core.h"

#include "jc/Encoding/CodePage.h"
#include "jc/Utils/Console.h"
#include "jc/Primitives/Atomic.h"

#pragma warning(push, 0)
#include <benchmark/benchmark.h>
#include <iostream>
#include <thread>
#include <cassert>
#include <unordered_map>
#pragma warning(pop)

USING_NS_JC;
USING_NS_STD;
USING_NS_BENCHMARK;

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
#define HashMapMemoryPoolTest	OFF
#define HashMapIterationTest	OFF
#define HeapifyTest				OFF

// ========================================================================================
// Pool
// ========================================================================================
#define IndexMemoryPoolTest		OFF

// ========================================================================================
// Threading
// ========================================================================================
#define CoroutineTest			OFF

// ========================================================================================
// Primitives (String 후보 벤치: R0=원본, S0~=후보, R1=std::string 참조군 / B1~=시나리오)
// ========================================================================================
#define StringBenchEnabled			ON		// String 벤치 전체 수행 여부

#if StringBenchEnabled == ON
	// 시나리오 파일 단위
	#define StringLifetimeBenchTest		ON	// B1~B4
	#define StringAppendBenchTest		ON	// B5~B8
	#define StringContainerBenchTest	ON	// B9~B10
	#define StringThreadBenchTest		ON	// B11
	// 시나리오 개별
	#define BenchStrB1					ON	// Construct
	#define BenchStrB2					ON	// CopyConstruct
	#define BenchStrB3					ON	// CopyAssign
	#define BenchStrB4					ON	// Move
	#define BenchStrB5					ON	// Append1B
	#define BenchStrB6					ON	// ConcatChain
	#define BenchStrB7					ON	// Format
	#define BenchStrB8					ON	// FindCompare
	#define BenchStrB9					ON	// VectorSort
	#define BenchStrB10					ON	// BinarySearch
	#define BenchStrB11					ON	// MTIndependent
	#define BenchStrB12					ON	// CrossThread
	#define BenchStrB13					ON	// OwnerExit
	// 후보 개별
	#define BenchStrR0					ON	// jc::String 원본
	#define BenchStrS0					ON	// 현행 복사본
	#define BenchStrS1					ON	// inline SSO 24B
	#define BenchStrS2					ON	// StringImpl* 8B
	#define BenchStrS3					ON	// + TLS pool
	#define BenchStrS4					ON	// + Impl SSO
	#define BenchStrS5					ON	// + owner 비교 / global fallback
	#define BenchStrS6					ON	// + deferred queue
	#define BenchStrS7					ON	// + Pool Storage
	#define BenchStrR1					ON	// std::string 참조군
#endif