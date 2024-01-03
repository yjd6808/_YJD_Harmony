/*
 * 작성자: 윤정도
 * 생성일: 01/02/2024 10:13:00 AM
 * =====================
 * PCH 파일
 */


#pragma once

#include <JNetwork/Network.h>
#include <JNetwork/Host/TcpClient.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/NetGroup.h>

#include <JCore/Sync/AutoResetEvent.h>

#include <Benchmark/Namespace.h>

#pragma warning(push, 0)
#include <benchmark/benchmark.h>
#include <iostream>
#include <thread>
#include <cassert>
#pragma warning(pop)

USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;
USING_NS_BENCHMARK;

// 테스트 IP:포트
#define JNET_RESEARCH_CONN_ADDR		"112.163.241.175:9999"
#define JNET_RESEARCH_BIND_ADDR		"0.0.0.0:9999"

#define ON							1
#define OFF							0

#define BufferingPerformanceTest	ON
#define BufferingWithNagleTest		OFF
#define PacketPoolingTest			OFF