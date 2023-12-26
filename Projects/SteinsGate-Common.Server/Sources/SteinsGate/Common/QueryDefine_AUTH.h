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
static constexpr const char* Script = "select c_uid, c_account_id, c_account_pass, c_created, c_logined from t_account";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectAccountInfoListResult)
Int32 AccountId;
JCore::String Id;
JCore::String Pass;
JCore::DateTime Created;
JCore::DateTime Logined;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	SelectAccountInfoListResult
	, AccountId, "c_uid"
	, Id, "c_account_id"
	, Pass, "c_account_pass"
	, Created, "c_created"
	, Logined, "c_logined"
);



// ==================================================================
// SelectAccountInfo
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(SelectAccountInfo)
static constexpr const char* Script = "select c_account_id, c_account_pass, c_created, c_logined, c_last_login_server from t_account where c_account_id = ?";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectAccountInfoResult)
JCore::String Id;
JCore::String Pass;
JCore::DateTime Created;
JCore::DateTime Logined;
Int32 LastServer;
QRY_SELECT_RESULT_END


REGISTER_QRY_STRUCT
(
	SelectAccountInfoResult
	, Id, "c_account_id"
	, Pass, "c_account_pass"
	, Created, "c_created"
	, Logined, "c_logined"
	, LastServer, "c_last_login_server"
);

// ==================================================================
// InsertAccountInfo
// ==================================================================

QRY_INSERT_STATEMENT_BEGIN(InsertAccountInfo)
static constexpr const char* Script = "insert into t_account (c_account_id, c_account_pass) values (?, ?)";
QRY_INSERT_STATEMENT_END

NS_QRY_END

