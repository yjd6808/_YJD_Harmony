#define _WINSOCKAPI_

#include <TF/Parser/SendFn.h>
#include <Common/Command.h>
#include <TF/Game/Player.h>

#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>
#include <TF/Game/Channel.h>

using namespace JNetwork;
using namespace JCore;


#define _Database	MysqlDatabase::GetInstance()
#define _World		World::GetInstance()


void SendFn::SendHardDisconnectSyn(Player* player) {
	
}

// LoadCharacterInfoAck 패킷 전달
// 캐릭터 정보들을 클라한테 전달함
void SendFn::SendLoadCharacterInfoAck(Player* player) {
	const auto pPacket = new Packet<LoadCharacterInfoAck>;
	LoadCharacterInfoAck* pLoadCharacterInfoAck = pPacket->Get<0>();

	const auto spQueryResult = _Database->Query("select * from t_character where c_account_id = ? and c_channed_uid = ?", player->GetAccountUID(), player->GetChannelUID());
	const int iCharacterCount = spQueryResult->GetResultRowCount();
	pLoadCharacterInfoAck->Count = iCharacterCount;
	for (int i = 1; i <= iCharacterCount; i++) {
		CharacterInfo* pCharacterInfo = &pLoadCharacterInfoAck->Info[i];

		pCharacterInfo->UID = spQueryResult->GetInt(i, 0);
		strcpy_s(pCharacterInfo->Name, NAME_LEN, spQueryResult->GetString(i, 3).Source());
		pCharacterInfo->Win = spQueryResult->GetInt(i, 4);
		pCharacterInfo->Lose = spQueryResult->GetInt(i, 5);
		pCharacterInfo->Kill = spQueryResult->GetInt(i, 6);
		pCharacterInfo->Death = spQueryResult->GetInt(i, 7);
	}

	player->Session()->SendAsync(pPacket);
}
