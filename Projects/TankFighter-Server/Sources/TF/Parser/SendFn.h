/*
 * �ۼ��� : ������
 * ���� ����ϴ� ��Ŷ �۽� �Լ�
 */

#pragma once

#include <TF/Game/Player.h>

struct SendFn
{
	static void SendHardDisconnectSyn(Player* player);
	static void SendLoadCharacterInfoAck(Player* player);
};

