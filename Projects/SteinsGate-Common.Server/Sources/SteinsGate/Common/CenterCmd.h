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


// ======================= CMD LIST =======================
// CmdWhoAreYou
// CmdItsMe
// CmdAlreadyConnected
// CmdYouNeedToDoThis
// CmdCenterMessage
// CmdNotifyBootState
// ======================= CMD LIST =======================

#pragma pack(push, CMD_ALIGNMENT)

StaticCmdBegin	
	( CmdWhoAreYou
	, 100
	, eServerToClient
	)
// ===========================================
// 필드 없음
// ===========================================
StaticCmdEnd(CmdWhoAreYou)

StaticCmdBegin
	( CmdItsMe
	, 101
	, eClientToServer
	)
// ===========================================
CenterClientType_t ClientType;
// ===========================================
StaticCmdEnd(CmdItsMe)


StaticCmdBegin
	( CmdAlreadyConnected
	, 102
	, eServerToClient
	)
// ===========================================
// 필드 없음
// ===========================================
StaticCmdEnd(CmdAlreadyConnected)


StaticCmdBegin
	( CmdYouNeedToDoThis
	, 103
	, eServerToClient
	)
// ===========================================
CenterOrder_t Order;
// ===========================================
StaticCmdEnd(CmdYouNeedToDoThis)

DynamicCmdBegin
	( CmdCenterMessage
	, 104
	, eClientToServer | eClientToServer
	, char 
	)
// ===========================================
SGStaticString<1> Msg;
// ===========================================
DynamicCmdEnd(CmdCenterMessage)



StaticCmdBegin
	( CmdNotifyBootState
	, 105
	, eClientToServer
	)
// ===========================================
ServerBootState_t State;
// ===========================================
StaticCmdEnd(CmdNotifyBootState)

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)