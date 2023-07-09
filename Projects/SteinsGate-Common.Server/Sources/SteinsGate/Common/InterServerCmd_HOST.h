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
// CmdHostMessage			: 서버/클라간 주고받는 문자열 메시지
// CmdNotifyBootState		: 클라가 현재 자신의 부팅상태를 서버에게 알려줌
// CmdTimeSync				: 시간 정보 요청
// CmdTimeSyncAck			: 시간 정보 요청 응답
// ======================= CMD LIST =======================

#define CMDID_CmdWhoAreYou					50000
#define CMDID_CmdItsMe						50001
#define CMDID_CmdAlreadyConnected			50002
#define CMDID_CmdYouNeedToDoThis			50003
#define CMDID_CmdHostMessage				50004
#define CMDID_CmdNotifyBootState			50005
#define CMDID_CmdTimeSync					50006
#define CMDID_CmdTimeSyncAck				50007

#pragma pack(push, CMD_ALIGNMENT)

HOST_STATIC_CMD_BEGIN
	( CmdWhoAreYou
	, CMDID_CmdWhoAreYou
	, eCenterToPeer
	)
// ───────────────────────────────────────────
// 필드 없음
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdWhoAreYou)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdItsMe
	, CMDID_CmdItsMe
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
	, CMDID_CmdAlreadyConnected
	, eCenterToPeer
	)
// ───────────────────────────────────────────
// 필드 없음
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdAlreadyConnected)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdYouNeedToDoThis
	, CMDID_CmdYouNeedToDoThis
	, eCenterToPeer
	)
// ───────────────────────────────────────────
CenterOrder_t Order;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdYouNeedToDoThis)

HOST_DYNAMIC_CMD_BEGIN
	( CmdHostMessage
	, CMDID_CmdHostMessage
	, eCenterToPeer | ePeerToCenter
	, JCore::StaticString<1>
	)
// ───────────────────────────────────────────
JCore::StaticString<1> Msg;
// ───────────────────────────────────────────
HOST_DYNAMIC_CMD_END(CmdHostMessage)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdNotifyBootState
	, CMDID_CmdNotifyBootState
	, ePeerToCenter
	)
// ───────────────────────────────────────────
ServerBootState_t State;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdNotifyBootState)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdTimeSync
	, CMDID_CmdTimeSync
	, ePeerToCenter
	)
// ───────────────────────────────────────────
JCore::DateTime PeerServerTime;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdTimeSync)

// ======================================================================================

HOST_STATIC_CMD_BEGIN
	( CmdTimeSyncAck
	, CMDID_CmdTimeSyncAck
	, eCenterToPeer
	)
// ───────────────────────────────────────────
JCore::DateTime PeerServerTime;
JCore::DateTime MasterServerTime;
// ───────────────────────────────────────────
HOST_STATIC_CMD_END(CmdTimeSyncAck)

// ======================================================================================

#pragma pack(pop)	// #pragma pack(push, CMD_ALIGNMENT)