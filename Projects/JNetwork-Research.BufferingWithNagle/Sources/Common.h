/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 * Heavy Traffic, Light Traffic 둘중 하나 주석 해제해서 테스트해보면 된다.^^
 */


#pragma once

#include <JCore/Primitives/Atomic.h>
#include <JNetwork/Packet/Command.h>



// Heavy-Traffic
#define TEST_CLIENT_COUNT		10			// 테스트 송신 클라이언트 수
#define TEST_SEND_COUNT			10'000		// 클라이언트당 테스트 에코
#define TEST_PADDING_SIZE		1'000
#define TEST_COUNT				1

// Middle-Traffic
//#define TEST_CLIENT_COUNT		1		// 테스트 송신 클라이언트 수
//#define TEST_SEND_COUNT			100		// 클라이언트당 테스트 에코
//#define TEST_PADDING_SIZE		50
//#define TEST_COUNT				10

// Light-Traffic 와이어샤크 캡쳐해서 분석하는 용도
// #define TEST_CLIENT_COUNT		1			// 테스트 송신 클라이언트 수
// #define TEST_SEND_COUNT			10			// 클라이언트당 테스트 에코 횟수
// #define TEST_PADDING_SIZE		0
// #define TEST_COUNT				1

// 1: Nonblocking + Overlapped Send
// 0: Blocking + Send
#define ASYNC_SENDING			1

 // 테스트 IP:포트
#define JNET_RESEARCH_CONN_ADDR		"112.163.241.175:9999"
#define JNET_RESEARCH_BIND_ADDR		"0.0.0.0:9999"

// 커맨드 ID
#define CMDID_SC_TEST			100
#define CMDID_CS_TEST			101

extern volatile bool TestFinished_v;
extern bool DontEcho_v;
extern JCore::StopWatch<JCore::StopWatchMode::HighResolution> StopWatch_v;
extern JCore::TimeSpan LastTime_v;

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