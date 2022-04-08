/*
 * �ۼ��� : ������
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
	// Ŭ��κ��� �α��� ��û ����
	static void CmdLoginSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ȸ������ ��û ����
	static void CmdRegisterSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ä�� ���� ��û ����
	static void CmdLoadChannelInfoSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ä�� ���� ��û ����
	static void CmdSelectChannelSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ���� ä���� ĳ���� ���� ��û ����
	static void CmdLoadCharacterInfoSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ĳ���� ���� ��û ����
	static void CmdSelectCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ĳ���� ���� ��û ����
	static void CmdCreateCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ĳ���� ���� ��û ����
	static void CmdDeleteCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� �κ� ���� �ߴ� ����
	static void CmdJoinLobbySyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� �� ���� ��û ����
	static void CmdCreateRoomSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� �� ���� ��û ����
	static void CmdJoinRoomSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ģ�� �߰� ��û ����
	static void CmdAddFriendSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ģ�� ���� ��û ����
	static void CmdDeleteFriendSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ģ�� ��û ����/�ź� ��� ����
	static void CmdAddFriendRequestAck(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� �� �� �ε��� �Ϸ��ؼ� ���� ��û ����
	static void CmdLoadRoomInfoSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ���� ���� ��û ����
	static void CmdRoomGameStartSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ���� �غ� ��û ����
	static void CmdRoomGameReadySyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� �غ� ���� ��û ����
	static void CmdRoomGameReadyCancelSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� �� ������ ��û ����
	static void CmdRoomLeaveSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ��κ��� ��Ʋ �ʵ忡 �����ߴ� ����
	static void CmdBattleFieldLoadSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ�� ��Ʋ�ʵ忡�� �ڽ��� ���������� �ֱ������� ������ �۽�����
	static void CmdBattileFieldTankMoveSyn(Player* player, JNetwork::ICommand* cmd);

	// Ŭ�� ��Ʋ�ʵ忡�� ������ Ż�ָ� �õ��Ѵٰ� ��
	static void CmdBattleFieldLeaveSyn(Player* player, JNetwork::ICommand* cmd);
	

	static void CmdTankMoveSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdTankMoveAck(Player* player, JNetwork::ICommand* cmd);
	
};
