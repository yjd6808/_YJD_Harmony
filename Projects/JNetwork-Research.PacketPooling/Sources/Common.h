/*
 * 작성자: 윤정도
 * 생성일: 01/02/2024 12:00:05 AM
 * =====================
 */


#pragma once

#include <JCore/Primitives/Atomic.h>
#include <JNetwork/Packet/Command.h>
#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Pool/BinarySearchMemoryPool.h>

// Test Traffic
#define TEST_CLIENT_COUNT		10			// 테스트 송신 클라이언트 수
#define TEST_SEND_COUNT			100'000		// 클라이언트당 테스트 에코
#define TEST_PADDING_SIZE		1'000
#define TEST_COUNT				3

 // 테스트 IP:포트
#define JNET_RESEARCH_CONN_ADDR		"112.163.241.175:9999"
#define JNET_RESEARCH_BIND_ADDR		"0.0.0.0:9999"

// 커맨드 ID
#define CMDID_SC_TEST			100
#define CMDID_CS_TEST			101

extern volatile bool TestFinished_v;
extern bool UsePooling_v;
extern JCore::StopWatch<JCore::StopWatchMode::HighResolution> StopWatch_v;
extern JCore::TimeSpan LastTime_v;
extern thread_local JCore::SharedPtr<JCore::BinarySearchMemoryPool> Pool_v;

#pragma pack(push, CMD_ALIGNMENT)

STATIC_CMD_BEGIN(CS_TEST, CMDID_CS_TEST)
int Seq;
char Data[TEST_PADDING_SIZE];
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_TEST, CMDID_SC_TEST)
int Ack;
char Data[TEST_PADDING_SIZE];
STATIC_CMD_END

#pragma pack(pop)
