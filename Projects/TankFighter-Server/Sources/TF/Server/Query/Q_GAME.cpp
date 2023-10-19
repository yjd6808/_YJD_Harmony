/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:25:54 AM
 * =====================
 *
 */


#include "Pch.h"
#include "Q_GAME.h"

#include <TF/Server/Contents/Character.h>

bool Q_GAME::CheckAccountExist(JCORE_REF_OUT int& accountPrimaryKey, const char* id, const char* pass) {
	Qry::SelectAccountInfoResult result;
	Qry::SelectAccountInfo::Execute<THelper>(Core::GameDB, result, id, pass);

	// 쿼리는 성공했지만 바인딩된 결과물이 없으면 실패로 간주
	if (!IsSuccess || !result.HasBindedResult)
		return false;

	accountPrimaryKey = result.PrimaryKey;

	return true;
}

bool Q_GAME::InsertAccountInfo(const char* id, const char* pass) {
	Qry::InsertResult result;
	Qry::InsertAccountInfo::Execute<THelper>(Core::GameDB, result, id, pass);
	return IsSuccess;
}

void Q_GAME::UpdateLoginDate(int accountPrimaryKey) {
	Qry::UpdateResult result;
	Qry::UpdateLoginDate::Execute<THelper>(Core::GameDB, result, JCore::DateTime::Now(), accountPrimaryKey);
}

bool Q_GAME::CheckAccountIdExist(const char* id, JCORE_OUT_OPT int* accountPrimaryKey /* = nullptr */) {
	Qry::CheckAccountIdExistResult result;
	Qry::CheckAccountIdExist::Execute<THelper>(Core::GameDB, result, id);

	if (!IsSuccess || !result.HasBindedResult)
		return false;

	if (accountPrimaryKey)
		*accountPrimaryKey = result.Uid;

	return true;
}

Qry::SelectChannelInfoResult Q_GAME::SelectChannelInfo() {
	Qry::SelectChannelInfoResult result;
	Qry::SelectChannelInfo::Execute<THelper>(Core::GameDB, result);
	return result;
}

Qry::SelectCharacterInfoListResult Q_GAME::SelectCharacterInfoList(int accountPrimaryKey, int channelPrimaryKey) {
	Qry::SelectCharacterInfoListResult result;
	Qry::SelectCharacterInfoList::Execute<THelper>(Core::GameDB, result, accountPrimaryKey, channelPrimaryKey);
	return result;
}

Qry::SelectCharacterInfoResult Q_GAME::SelectCharacterInfo(int accountPrimaryKey, int channelPrimaryKey, int characterPrimaryKey) {
	Qry::SelectCharacterInfoResult result;
	Qry::SelectCharacterInfo::Execute<THelper>(Core::GameDB, result, accountPrimaryKey, channelPrimaryKey, characterPrimaryKey);
	return result;
}

bool Q_GAME::CheckCharacterNameExist(const char* name, int channelPrimaryKey) {
	Qry::CheckCharacterNameExistResult result;
	Qry::CheckCharacterNameExist::Execute<THelper>(Core::GameDB, result, name, channelPrimaryKey);
	return result.HasBindedResult;
}

bool Q_GAME::CreateCharacter(int accountPrimaryKey, int channelPrimaryKey, const char* name, int win, int lose, int kill, int death, int money) {
	Qry::InsertResult result;
	Qry::CreateCharacter::Execute<THelper>(Core::GameDB, result, accountPrimaryKey, channelPrimaryKey, name, win, lose, kill, death, money);
	return IsSuccess;
}

bool Q_GAME::DeleteCharacter(int accountPrimaryKey, int channelPrimaryKey, const char* name) {
	Qry::DeleteResult result;
	Qry::DeleteCharacter::Execute<THelper>(Core::GameDB, result, accountPrimaryKey, channelPrimaryKey, name);
	return result.RowCount > 0;
}

Qry::SelectFriendCharacterInfoListResult Q_GAME::SelectFriendCharacterInfoList(int characterPrimaryKey) {
	Qry::SelectFriendCharacterInfoListResult result;
	Qry::SelectFriendCharacterInfoList::Execute<THelper>(Core::GameDB, result, characterPrimaryKey, characterPrimaryKey);
	return result;
}

Qry::CheckFriendResult Q_GAME::CheckFriend(int lhsCharacterPrimaryKey, int rhsCharacterPrimaryKey) {
	Qry::CheckFriendResult result;
	Qry::CheckFriend::Execute<THelper>(Core::GameDB, result, lhsCharacterPrimaryKey, rhsCharacterPrimaryKey, rhsCharacterPrimaryKey, lhsCharacterPrimaryKey);
	return result;
}

bool Q_GAME::AddFriendship(int requestCharacterPrimaryKey, int acceptCharacterPrimaryKey) {
	Qry::InsertResult result;
	Qry::AddFriendship::Execute<THelper>(Core::GameDB, result, requestCharacterPrimaryKey, acceptCharacterPrimaryKey);
	return IsSuccess;
}

bool Q_GAME::DeleteFriend(int lhsCharacterPrimaryKey, int rhsCharacterPrimaryKey, JCORE_OUT int* deletedCount /*= nullptr*/) {
	Qry::DeleteResult result;
	Qry::DeleteFriend::Execute<THelper>(Core::GameDB, result, lhsCharacterPrimaryKey, rhsCharacterPrimaryKey, rhsCharacterPrimaryKey, lhsCharacterPrimaryKey);

	if (IsSuccess && deletedCount)
		*deletedCount = result.RowCount;

	return IsSuccess;
}
