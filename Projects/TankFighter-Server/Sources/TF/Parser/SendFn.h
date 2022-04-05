/*
 * 작성자 : 윤정도
 * 자주 사용하는 패킷 송신 함수
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

	// player에게 자신의 캐릭터 정보 전송
	static void SendUpdateCharacterInfoAck(Player* player);

	// player에게 방 리스트 정보 전송
	static void SendUpdateRoomListAck(Player* player, int channelUID);

	// player에게 친구로 등록된 캐릭터 정보들 전송
	static void SendUpdateFriendListAck(Player* player, int characterUID);

	// player에게 서버 메시지 전송
	static void SendServerMessageSyn(Player* player, const JCore::String& message);

	// 로비에 있는 모든 플레이어에게 방 리스트 정보 전송
	static void BroadcastUpdateRoomListAck(Channel* channel);
	static void BroadcastUpdateRoomUserAck(Room* room);
};

