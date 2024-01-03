/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/Atomic.h>
#include <JNetwork/Packet/Command.h>

#define TEST_CLIENT_COUNT		10			// 테스트 송신 클라이언트 수
#define TEST_SEND_COUNT			100'000		// 클라이언트당 테스트 에코 횟수

 // 테스트 IP:포트
#define JNET_RESEARCH_CONN_ADDR		"112.163.241.175:9999"
#define JNET_RESEARCH_BIND_ADDR		"0.0.0.0:9999"

// 커맨드 ID
#define CMDID_SC_TEST			100
#define CMDID_CS_TEST			101


STATIC_CMD_BEGIN(CS_TEST, CMDID_CS_TEST)
int Seq;
char Data[1020];
STATIC_CMD_END

STATIC_CMD_BEGIN(SC_TEST, CMDID_SC_TEST)
int Ack;
char Data[1020];
STATIC_CMD_END