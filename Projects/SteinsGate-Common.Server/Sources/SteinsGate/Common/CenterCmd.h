/*
 * 작성자: 윤정도
 * 생성일: 3/31/2023 10:19:48 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/Command.h>
#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/ServerEnum.h>

#define CmdWhoAreYou_SC						100		// 너는 누구냐?
#define CmdItsMe_CS							101		// 내다.
#define CmdAlreadyConnected_SC				102		// 넌 이미 연결되어있다.
#define CmdYouNeedToDoThis_SC				103		// 넌 이걸 해야해
#define CmdCenterMessage_CS					104
#define CmdNotifyBootState_CS				105		// 현재 서버의 부팅 상태

#pragma pack(push, CMD_ALIGNMENT)

StaticCmdBegin(CmdWhoAreYou, CmdWhoAreYou_SC)
StaticCmdEnd(CmdWhoAreYou)

StaticCmdBegin(CmdItsMe, CmdItsMe_CS)
CenterClientType_t ClientType;
StaticCmdEnd(CmdItsMe)

StaticCmdBegin(CmdAlreadyConnected, CmdAlreadyConnected_SC)
StaticCmdEnd(CmdAlreadyConnected)

StaticCmdBegin(CmdYouNeedToDoThis, CmdYouNeedToDoThis_SC)
CenterOrder_t Order;
StaticCmdEnd(CmdYouNeedToDoThis)

DynamicCmdBegin(CmdCenterMessage, CmdCenterMessage_CS, char)
SGStaticString<1> Msg;
DynamicCmdEnd(CmdCenterMessage)

StaticCmdBegin(CmdNotifyBootState, CmdNotifyBootState_CS)
ServerBootState_t State;
StaticCmdEnd(CmdNotifyBootState)

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)