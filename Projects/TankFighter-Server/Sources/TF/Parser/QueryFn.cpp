#define _WINSOCKAPI_

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
	auto spQuery = _Database->Query("select count(*) from t_character where c_name = ?", characterName);
	return spQuery->GetResultRowCount() > 0;
}

bool QueryFn::IsCharacterExistByIDs(const int accountUID, const int channelUID, const int characterUID) {
	auto spQuery = _Database->Query("select count(*) from t_character where c_account_uid = ? and c_channel_uid = ? and c_uid = ?", 
		accountUID, channelUID, characterUID);
	return spQuery->GetResultRowCount() > 0;
}
