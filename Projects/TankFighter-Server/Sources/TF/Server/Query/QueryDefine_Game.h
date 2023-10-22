/*
 * 작성자: 윤정도
 * 생성일: 10/16/2023 9:57:24 PM
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
static constexpr const char* Script = QRY_SCRIPT(
	select c_account_id, c_id, c_pass, c_created, c_logined 
	 from t_account
)
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
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid, c_id, c_pass 
	 from t_account 
    where c_id = ? and c_pass = md5(?)
)
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
static constexpr const char* Script = QRY_SCRIPT(
	insert into t_account (c_id, c_pass) values (?, md5(?))
)
QRY_INSERT_STATEMENT_END


// ==================================================================
// UpdateLoginDate
// ==================================================================
QRY_UPDATE_STATEMENT_BEGIN(UpdateLoginDate)
static constexpr const char* Script = QRY_SCRIPT(
	update t_account set c_last_login_time = ? where c_uid = ?
)
QRY_UPDATE_STATEMENT_END


// ==================================================================
// CheckAccountIdExist
// ==================================================================
// mysql에서 행이 존재하는지 체크하는 가장 빠른 방법
// https://stackoverflow.com/questions/1676551/best-way-to-test-if-a-row-exists-in-a-mysql-table
QRY_SELECT_STATEMENT_BEGIN(CheckAccountIdExist)
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid 
	  from t_account 
     where c_id = ? limit 1
)
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
static constexpr const char* Script = QRY_SCRIPT(
	select * 
	 from t_channel
)
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectChannelInfoResult)
int PrimaryKey;
JCore::String Name;
int MaxPlayerCount;
JCore::DateTime GeneratedTime;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	SelectChannelInfoResult
	, PrimaryKey, "c_uid"
	, Name, "c_name"
	, MaxPlayerCount, "c_max_player_count"
	, GeneratedTime, "c_time"
);

// ==================================================================
// SelectCharacterInfoList
// ==================================================================
QRY_SELECT_STATEMENT_BEGIN(SelectCharacterInfoList)
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid, c_name, c_win, c_lose, c_kill, c_death, c_money 
	  from t_character 
     where c_account_uid = ? and c_channel_uid = ?
)
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectCharacterInfoListResult)
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
	SelectCharacterInfoListResult
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
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid 
	  from t_character 
     where c_name = ? and c_channel_uid = ? limit 1
)
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
static constexpr const char* Script = QRY_SCRIPT(
	insert into t_character (c_account_uid, c_channel_uid, c_name, c_win, c_lose, c_kill, c_death, c_money) 
	values (?, ?, ?, ?, ?, ?, ?, ?)
)
QRY_INSERT_STATEMENT_END

// ==================================================================
// DeleteCharacter
// ==================================================================
QRY_DELETE_STATEMENT_BEGIN(DeleteCharacter)
static constexpr const char* Script = QRY_SCRIPT(
	delete from t_character 
     where c_account_uid = ? and c_channel_uid = ? and c_name = ?
)
QRY_DELETE_STATEMENT_END


// ==================================================================
// SelectCharacterInfo
// ==================================================================
QRY_SELECT_STATEMENT_BEGIN(SelectCharacterInfo)
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid, c_name, c_win, c_lose, c_kill, c_death, c_money 
	  from t_character 
	 where c_account_uid = ? and c_channel_uid = ? and c_uid = ?
)
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
// CheckFriend
// ==================================================================
QRY_SELECT_STATEMENT_BEGIN(CheckFriend)
static constexpr const char* Script = QRY_SCRIPT(
	select count(*) as c_count 
	  from t_friendship
	 where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_req_character_uid = ? and c_ack_character_uid = ?)
	 limit 1
)
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(CheckFriendResult)
int Count;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	CheckFriendResult
	, Count, "c_count"
);



// ==================================================================
// AddFriendship
// ==================================================================
QRY_INSERT_STATEMENT_BEGIN(AddFriendship)
static constexpr const char* Script = QRY_SCRIPT(
	insert into t_friendship (c_req_character_uid, c_ack_character_uid) 
	values (?, ?)
)
QRY_INSERT_STATEMENT_END


// ==================================================================
// SelectFriendCharacterInfoList
// ==================================================================
QRY_SELECT_STATEMENT_BEGIN(SelectFriendCharacterInfoList)
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid, c_name
	  from t_character as A
      join (
		   select c_req_character_uid as c_uid2 from t_friendship where c_ack_character_uid = ?
		   union
		   select c_ack_character_uid as c_uid2 from t_friendship where c_req_character_uid = ?
	  ) as B on A.c_uid = B.c_uid2
)
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectFriendCharacterInfoListResult)
int PrimaryKey;
JCore::String Name;
QRY_SELECT_RESULT_END

REGISTER_QRY_STRUCT
(
	SelectFriendCharacterInfoListResult
	, PrimaryKey, "c_uid"
	, Name, "c_name"
);

// ==================================================================
// SelectFriendCharacterInfo
// ==================================================================
QRY_SELECT_STATEMENT_BEGIN(SelectFriendCharacterInfo)
static constexpr const char* Script = QRY_SCRIPT(
	select c_uid, c_name, c_win, c_lose, c_kill, c_death, c_money
	  from t_character as A
      join (
		   select c_req_character_uid as c_uid2 from t_friendship where c_ack_character_uid = ?
		   union
		   select c_ack_character_uid as c_uid2 from t_friendship where c_req_character_uid = ?
	  ) as B on A.c_uid = B.c_uid2
	 limit 1
)
QRY_SELECT_STATEMENT_END

QRY_SELECT_RESULT_BEGIN(SelectFriendCharacterInfoResult)
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
	SelectFriendCharacterInfoResult
	, PrimaryKey, "c_uid"
	, Name, "c_name"
	, Win, "c_win"
	, Lose, "c_lose"
	, Death, "c_death"
	, Money, "c_money"
);


// ==================================================================
// DeleteFriend
// ==================================================================
QRY_DELETE_STATEMENT_BEGIN(DeleteFriend)
static constexpr const char* Script = QRY_SCRIPT(
	delete from t_friendship 
	 where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_req_character_uid = ? and c_ack_character_uid = ?)
)
QRY_DELETE_STATEMENT_END

// ==================================================================
// AddKillCount
// ==================================================================
QRY_UPDATE_STATEMENT_BEGIN(AddKillCount)
static constexpr const char* Script = QRY_SCRIPT(
	update t_character set c_kill = c_kill + ?
where c_uid = ?
)
QRY_UPDATE_STATEMENT_END

// ==================================================================
// AddDeathCount
// ==================================================================
QRY_UPDATE_STATEMENT_BEGIN(AddDeathCount)
static constexpr const char* Script = QRY_SCRIPT(
	update t_character set c_death = c_death + ?
	 where c_uid = ?
)
QRY_UPDATE_STATEMENT_END


// ==================================================================
// AddWinCount
// ==================================================================
QRY_UPDATE_STATEMENT_BEGIN(AddWinCount)
static constexpr const char* Script = QRY_SCRIPT(
	update t_character set c_win = c_win + ? 
     where c_uid = ?
)
QRY_UPDATE_STATEMENT_END

// ==================================================================
// AddLoseCount
// ==================================================================
QRY_UPDATE_STATEMENT_BEGIN(AddLoseCount)
static constexpr const char* Script = QRY_SCRIPT(
	update t_character set c_lose = c_lose + ? 
     where c_uid = ?
)
QRY_UPDATE_STATEMENT_END


NS_QRY_END


