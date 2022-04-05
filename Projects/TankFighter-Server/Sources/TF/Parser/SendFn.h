/*
 * �ۼ��� : ������
 * ���� ����ϴ� ��Ŷ �۽� �Լ�
 */

#pragma once

namespace JCore { class String; }
class Channel;
class Player;
class Room;
struct SendFn
{
	static void SendHardDisconnectSyn(Player* player);
	static void SendLoadCharacterInfoAck(Player* player);

	// player���� �ڽ��� ĳ���� ���� ����
	static void SendUpdateCharacterInfoAck(Player* player);

	// player���� �� ����Ʈ ���� ����
	static void SendUpdateRoomListAck(Player* player, int channelUID);

	// player���� ģ���� ��ϵ� ĳ���� ������ ����
	static void SendUpdateFriendListAck(Player* player, int characterUID);

	// player���� ���� �޽��� ����
	static void SendServerMessageSyn(Player* player, const JCore::String& message);

	// �κ� �ִ� ��� �÷��̾�� �� ����Ʈ ���� ����
	static void BroadcastUpdateRoomListAck(Channel* channel);
	static void BroadcastUpdateRoomUserAck(Room* room);
};

