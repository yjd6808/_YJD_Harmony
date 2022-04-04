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


	//static void Callback(MysqlQuery* query);
};