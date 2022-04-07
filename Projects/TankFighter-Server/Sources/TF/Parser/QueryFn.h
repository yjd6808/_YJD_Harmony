/*
 * �ۼ��� : ������
 * ���� ����ϴ� ���� �Լ�
 */

#pragma once

#include <TF/Game/Player.h>
#include <TF/Database/MysqlQuery.h>

struct QueryFn
{
	static bool IsCharacterExistByName(const JCore::String& name);
	static bool IsCharacterExistByIDs(int accountUID, int channelUID, int characterUID);

	// �� ĳ���Ͱ� ���� ģ������.
	static bool IsCharacterFriend(int characterUID1, int characterUID2);

	// ĳ���� �г������� ĳ���� UID��������
	static int GetCharacterUIDByNickName(const JCore::String& nickName);

	//static void Callback(MysqlQuery* query);
};