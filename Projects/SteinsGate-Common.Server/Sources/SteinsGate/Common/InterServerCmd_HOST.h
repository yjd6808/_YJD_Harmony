/*
 * 작성자: 윤정도
 * 생성일: 3/31/2023 10:19:48 PM
 * =====================
 * 중앙 <-> 개별서버간 통신
 *
 * 개별서버는 여기서 클라라고 지칭하였고
 * 중앙서버는 여기서 서버라고 지칭함.
 */


#pragma once

#include <JCore/Primitives/StaticString.h>
#include <SteinsGate/Common/ServerEnum.h>
#include <SteinsGate/Common/InterServerCmdDefine.h>


// ======================= CMD LIST =======================
// CmdWhoAreYou				: 서버가 접속한 클라이언트가 누군지 물어봄
// CmdItsMe					: 클라가 누구인지 서버에게 알려줌
// CmdAlreadyConnected		: 서버가 접속한 클라이언트에게 넌 이미 접속중이라고 알려줌
// CmdYouNeedToDoThis		: 서버가 접속한 클라이언트에게 명령을 내림
// CmdInterServerMessage	: 서버/클라간 주고받는 문자열 메시지
// CmdNotifyBootState		: 클라가 현재 자신의 부팅상태를 서버에게 알려줌
// ======================= CMD LIST =======================

#pragma pack(push, CMD_ALIGNMENT)

HOST_STATIC_CMD_BEGIN
	( CmdWhoAreYou
	, 100
	, eCenterToPeer
	)
// ───────────────────────────────────────────
// 필드 없음
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdWhoAreYou)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdItsMe
	, 101
	, eClientToServer
	)
// ───────────────────────────────────────────
InterServerClientType_t ClientType;
int ServerId;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdItsMe)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdAlreadyConnected
	, 102
	, eCenterToPeer
	)
// ───────────────────────────────────────────
// 필드 없음
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdAlreadyConnected)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdYouNeedToDoThis
	, 103
	, eCenterToPeer
	)
// ───────────────────────────────────────────
CenterOrder_t Order;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdYouNeedToDoThis)

HOST_DYNAMIC_CMD_BEGIN
	( CmdInterServerMessage
	, 104
	, eCenterToPeer | ePeerToCenter
	, char 
	)
// ───────────────────────────────────────────
JCore::StaticString<1> Msg;
// ───────────────────────────────────────────
HOST_DYNAMIC_CMD_END(CmdInterServerMessage)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdNotifyBootState
	, 105
	, ePeerToCenter
	)
// ───────────────────────────────────────────
ServerBootState_t State;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdNotifyBootState)

// ======================================================================================

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)