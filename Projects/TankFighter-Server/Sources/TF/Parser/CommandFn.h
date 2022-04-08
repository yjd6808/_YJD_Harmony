/*
 * 작성자 : 윤정도
 */

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
	static void CmdAddFriendRequestAck(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 방 씬 로딩을 완료해서 정보 요청 수신
	static void CmdLoadRoomInfoSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 게임 시작 요청 수신
	static void CmdRoomGameStartSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 게임 준비 요청 수신
	static void CmdRoomGameReadySyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 준비 해제 요청 수신
	static void CmdRoomGameReadyCancelSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 방 나가기 요청 수신
	static void CmdRoomLeaveSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라로부터 배틀 필드에 진입했다 수신
	static void CmdBattleFieldLoadSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라가 배틀필드에서 자신의 상태정보를 주기적으로 서버로 송신해줌
	static void CmdBattileFieldTankMoveSyn(Player* player, JNetwork::ICommand* cmd);

	// 클라가 배틀필드에서 강제로 탈주를 시도한다고 함
	static void CmdBattleFieldLeaveSyn(Player* player, JNetwork::ICommand* cmd);
	

	static void CmdTankMoveSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdTankMoveAck(Player* player, JNetwork::ICommand* cmd);
	
};
