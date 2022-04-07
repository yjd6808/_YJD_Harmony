/*
 * 작성자 : 윤정도
 * 자주 사용하는 쿼리 함수
 */

#pragma once

#include <TF/Game/Player.h>
#include <TF/Database/MysqlQuery.h>

struct QueryFn
{
	static bool IsCharacterExistByName(const JCore::String& name);
	static bool IsCharacterExistByIDs(int accountUID, int channelUID, int characterUID);

	// 두 캐릭터가 서로 친구인지.
	static bool IsCharacterFriend(int characterUID1, int characterUID2);

	// 캐릭터 닉네임으로 캐릭터 UID가져오기
	static int GetCharacterUIDByNickName(const JCore::String& nickName);

	//static void Callback(MysqlQuery* query);
};