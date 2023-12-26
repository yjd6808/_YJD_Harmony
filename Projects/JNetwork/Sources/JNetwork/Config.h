/*
 * 작성자: 윤정도
 * 생성일: 12/25/2023 9:39:58 PM (크리스마스)
 * =====================
 *
 */


#pragma once

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

extern bool DisableSendBuffering;
extern bool DisableRecvBuffering;

extern int DefaultSendBufferSize;
extern int DefaultRecvBufferSize;

extern int DefaultParserType;

NS_JNET_END