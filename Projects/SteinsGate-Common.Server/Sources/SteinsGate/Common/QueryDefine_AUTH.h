/*
 * 작성자: 윤정도
 * 생성일: 5/23/2023 9:57:24 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/QueryDefine.h>

NS_QRY_BEGIN


// ==================================================================
// SelectAccountInfoList
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(SelectAccountInfoList)
static constexpr const char* Script = "select c_account_id, c_id, c_pass, c_created, c_logined from t_account";
QRY_SELECT_STATEMENT_END(SelectAccountInfoList)

QRY_SELECT_RESULT_BEGIN(SelectAccountInfoListResult)
Int32 AccountId;
JCore::String Id;
JCore::String Pass;
JCore::DateTime Created;
JCore::DateTime Logined;
QRY_SELECT_RESULT_END(SelectAccountInfoListResult)

REGISTER_QRY_STRUCT
(
	SelectAccountInfoListResult
	, AccountId, "c_account_id"
	, Id, "c_id"
	, Pass, "c_pass"
	, Created, "c_created"
	, Logined, "c_logined"
);



// ==================================================================
// SelectAccountInfo
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(SelectAccountInfo)
static constexpr const char* Script = "select c_id, c_pass, c_created, c_logined, c_last_server from t_account where c_id = ?";
QRY_SELECT_STATEMENT_END(SelectAccountInfo)

QRY_SELECT_RESULT_BEGIN(SelectAccountInfoResult)
JCore::String Id;
JCore::String Pass;
JCore::DateTime Created;
JCore::DateTime Logined;
Int32 LastServer;
QRY_SELECT_RESULT_END(SelectAccountInfoResult)


REGISTER_QRY_STRUCT
(
	SelectAccountInfoResult
	, Id, "c_id"
	, Pass, "c_pass"
	, Created, "c_created"
	, Logined, "c_logined"
	, LastServer, "c_last_server"
);

// ==================================================================
// InsertAccountInfo
// ==================================================================

QRY_INSERT_STATEMENT_BEGIN(InsertAccountInfo)
static constexpr const char* Script = "insert into t_account (c_id, c_pass) values (?, ?)";
QRY_INSERT_STATEMENT_END(InsertAccountInfo)

NS_QRY_END

