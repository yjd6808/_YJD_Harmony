/*
 * 작성자: 윤정도
 * 생성일: 5/23/2023 9:57:24 PM
 * =====================
 *
 */


#pragma once

#include <TF/Server/Database/QueryDefine.h>

NS_QRY_BEGIN

// ==================================================================
// SelectAccountInfoList
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(SelectAccountInfoList)
static constexpr const char* Script = "select c_account_id, c_id, c_pass, c_created, c_logined from t_account";
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
static constexpr const char* Script = "select c_uid, c_id, c_pass from t_account where c_id = ? and c_pass = md5(?)";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectAccountInfoResult)
int PrimaryKey;
JCore::String Id;
JCore::String Pass;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	SelectAccountInfoResult
	, PrimaryKey, "c_uid"
	, Id, "c_id"
	, Pass, "c_pass"
);

// ==================================================================
// InsertAccountInfo
// ==================================================================

QRY_INSERT_STATEMENT_BEGIN(InsertAccountInfo)
static constexpr const char* Script = "insert into t_account (c_id, c_pass) values (?, md5(?))";
QRY_INSERT_STATEMENT_END


// ==================================================================
// UpdateLoginDate
// ==================================================================

QRY_UPDATE_STATEMENT_BEGIN(UpdateLoginDate)
static constexpr const char* Script = "update t_account set c_last_login_time = ? where c_uid = ?";
QRY_UPDATE_STATEMENT_END


// ==================================================================
// CheckAccountIdExist
// ==================================================================

// mysql에서 행이 존재하는지 체크하는 가장 빠른 방법
// https://stackoverflow.com/questions/1676551/best-way-to-test-if-a-row-exists-in-a-mysql-table
QRY_SELECT_STATEMENT_BEGIN(CheckAccountIdExist)
static constexpr const char* Script = "select c_uid from t_account where c_id = ? limit 1";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(CheckAccountIdExistResult)
int Uid;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	CheckAccountIdExistResult
	, Uid, "c_uid"
);

// ==================================================================
// SelectChannelInfo
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(SelectChannelInfo)
static constexpr const char* Script = "select * from t_channel";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectChannelInfoResult)
int Uid;
JCore::String Name;
int MaxPlayerCount;
JCore::DateTime GeneratedTime;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	SelectChannelInfoResult
	, Uid, "c_uid"
	, Name, "c_name"
	, MaxPlayerCount, "c_max_player_count"
	, GeneratedTime, "c_time"
);

// ==================================================================
// SelectCharacterInfo
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(SelectCharacterInfo)
static constexpr const char* Script = "select c_uid, c_name, c_win, c_lose, c_kill, c_death, c_money from t_character where c_account_uid = ? and c_channel_uid = ?";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectCharacterInfoResult)
int PrimaryKey;
JCore::String Name;
int Win;
int Lose;
int Kill;
int Death;
int Money;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	SelectCharacterInfoResult
	, PrimaryKey, "c_uid"
	, Name, "c_name"
	, Win, "c_win"
	, Lose, "c_lose"
	, Kill, "c_kill"
	, Death, "c_death"
	, Money, "c_money"
);

// ==================================================================
// CheckCharacterNameExist
// ==================================================================

QRY_SELECT_STATEMENT_BEGIN(CheckCharacterNameExist)
static constexpr const char* Script = "select c_uid from t_character where c_name = ? and c_channel_uid = ? limit 1";
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(CheckCharacterNameExistResult)
int CharacterPrimaryKey;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	CheckCharacterNameExistResult
	, CharacterPrimaryKey, "c_uid"
);

// ==================================================================
// CreateCharacter
// ==================================================================

QRY_INSERT_STATEMENT_BEGIN(CreateCharacter)
static constexpr const char* Script = "insert into t_character (c_account_uid, c_channel_uid, c_name, c_win, c_lose, c_kill, c_death, c_money) values (?, ?, ?, ?, ?, ?, ?, ?)";
QRY_INSERT_STATEMENT_END

// ==================================================================
// DeleteCharacter
// ==================================================================

QRY_DELETE_STATEMENT_BEGIN(DeleteCharacter)
static constexpr const char* Script = "delete from t_character where c_account_uid = ? and c_channel_uid = ? and c_name = ?";
QRY_DELETE_STATEMENT_END

NS_QRY_END


