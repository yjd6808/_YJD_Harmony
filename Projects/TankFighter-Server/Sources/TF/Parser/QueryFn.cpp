/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Parser/QueryFn.h>
#include <Common/Command.h>
#include <TF/Game/Player.h>

#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>
#include <TF/Game/Channel.h>

using namespace JNetwork;
using namespace JCore;


#define _Database	MysqlDatabase::GetInstance()
#define _World		World::GetInstance()

bool QueryFn::IsCharacterExistByName(const JCore::String& characterName) {
	const auto spQuery = _Database->Query("select count(*) from t_character where c_name = ?", characterName);
	return spQuery->GetInt(0, 0) > 0;
}

bool QueryFn::IsCharacterExistByIDs(const int accountUID, const int channelUID, const int characterUID) {
	const auto spQuery = _Database->Query("select count(*) from t_character where c_account_uid = ? and c_channel_uid = ? and c_uid = ?", 
	                                      accountUID, channelUID, characterUID);
	return spQuery->GetResultRowCount() > 0;
}

bool QueryFn::IsCharacterFriend(int characterUID1, int characterUID2) {
	return _Database->Query("select * from t_friendship where (c_req_character_uid = ? and c_ack_character_uid = ?) or (c_ack_character_uid = ? and c_req_character_uid = ?)",
		characterUID1, characterUID2, characterUID1, characterUID2)->GetResultRowCount() > 0;
}

int QueryFn::GetCharacterUIDByNickName(const JCore::String& nickName) {
	const auto spQuery = _Database->Query("select u_id from t_character where c_name = ?", nickName);

	if (spQuery->GetResultRowCount()) {
		return spQuery->GetInt(0, 0);
	}

	return INVALID_UID;
}

void QueryFn::AddKillCountAsync(int characterUID, int count) {
	_Database->QueryAsync("update t_character set c_kill = c_kill + ? where c_uid = ?", count, characterUID)->Release();
}

void QueryFn::AddDeathCountAsync(int characterUID, int count) {
	_Database->QueryAsync("update t_character set c_death = c_death + ? where c_uid = ?", count, characterUID)->Release();
}

void QueryFn::AddWinCountAsync(int characterUID, int count) {
	_Database->QueryAsync("update t_character set c_win = c_win + ? where c_uid = ?", count, characterUID)->Release();
}

void QueryFn::AddLoseCountAsync(int characterUID, int count) {
	_Database->QueryAsync("update t_character set c_lose = c_lose + ? where c_uid = ?", count, characterUID)->Release();
}
