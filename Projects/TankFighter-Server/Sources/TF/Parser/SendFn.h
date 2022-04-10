/*
 * �ۼ��� : ������
 * ���� ����ϴ� ��Ŷ �۽� �Լ�
 */

#pragma once

#include <JCore/Type.h>

namespace JCore { class String; }
class Channel;
class Player;
class Room;
struct SendFn
{
	static void SendLoadCharacterInfoAck(Player* player);

	// player���� �ڽ��� ĳ���� ���� ����
	static void SendUpdateCharacterInfoAck(Player* player);

	// player���� �� ����Ʈ ���� ����
	static void SendUpdateRoomListAck(Player* player, int channelUID);

	// player���� ģ���� ��ϵ� ĳ���� ������ ����
	static void SendUpdateFriendListAck(Player* player, int characterUID);

	// player���� ���� �޽��� ����
	static void SendServerMessageSyn(Player* player, const JCore::String& message);

	// player �� ���� ����
	static void SendRoomInfoAck(Room* room, Player* player);

	// �κ� �ִ� ��� �÷��̾�� �� ����Ʈ ���� ����
	static void BroadcastUpdateRoomListAck(Channel* channel);
	static void SendRTTAck(Player* player, Int64U tick);

	// �濡 �ִ� ��� �ÿ��̾�� ���� �濡 ����ִ� �÷��̾� ������ ����
	static void BroadcastUpdateRoomUserAck(Room* room, bool unsafe = false);
	
};

