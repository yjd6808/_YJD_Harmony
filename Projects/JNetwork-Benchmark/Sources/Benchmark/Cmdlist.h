/*
 * 작성자: 윤정도
 * 생성일: 01/02/2024 10:13:00 AM
 * =====================
 * PCH 파일
 */


#pragma once

#include <JNetwork/Packet/Command.h>

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
