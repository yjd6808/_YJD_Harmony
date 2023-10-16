/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:25:47 AM
 * =====================
 *
 */


#pragma once


#include <TF/Server/Database/QueryHelper.h>
#include <TF/Server/Query/QueryDefine_Game.h>

struct Q_GAME : Qry::QryHelper<Q_GAME>
{
	static bool CheckAccountExist(JCORE_REF_OUT int& accountPrimaryKey, const char* id, const char* pass);
	static bool InsertAccountInfo(const char* id, const char* pass);
	static void UpdateLoginDate(int accountPrimaryKey);
	static bool CheckAccountIdExist(const char* id, JCORE_OUT_OPT int* accountPrimaryKey = nullptr);
	static Qry::SelectChannelInfoResult SelectChannelInfo();
	static Qry::SelectCharacterInfoResult SelectCharacterInfo(int accountPrimaryKey, int channelPrimaryKey);
	static bool CheckCharacterNameExist(const char* name, int channelPrimaryKey);
	static bool CreateCharacter(int accountPrimaryKey, int channelPrimaryKey, const char* name, int win, int lose, int kill, int death, int money);
	static bool DeleteCharacter(int accountPrimaryKey, int channelPrimaryKey, const char* name);
};

