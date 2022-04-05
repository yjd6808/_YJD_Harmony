#pragma once

class Player;

namespace JNetwork {
	struct ICommand;
	class TcpSession;
}

class CommandFn
{
public:
	// 클라로부터 로그인 요청 수신
	static void CmdLoginSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 회원가입 요청 수신
	static void CmdRegisterSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 채널 정보 요청 수신
	static void CmdLoadChannelInfoSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 채널 선택 요청 수신
	static void CmdSelectChannelSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 현재 채널의 캐릭터 정보 요청 수신
	static void CmdLoadCharacterInfoSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 캐릭터 선택 요청 수신
	static void CmdSelectCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 캐릭터 생성 요청 수신
	static void CmdCreateCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 캐릭터 삭제 요청 수신
	static void CmdDeleteCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 로비 참가 했다 수신
	static void CmdJoinLobbySyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 방 생성 요청 수신
	static void CmdCreateRoomSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 방 참가 요청 수신
	static void CmdJoinRoomSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 친구 추가 요청 수신
	static void CmdAddFriendSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 친구 삭제 요청 수신
	static void CmdDeleteFriendSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 친구 요청 수락/거부 결과 수신
	static void CmdFriendRequestAck(Player* player, JNetwork::ICommand* cmd);

	static void CmdTankMoveSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdTankMoveAck(Player* player, JNetwork::ICommand* cmd);
};
