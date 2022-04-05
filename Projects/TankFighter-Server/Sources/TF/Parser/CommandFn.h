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
	static void CmdFriendRequestAck(Player* player, JNetwork::ICommand* cmd);

	static void CmdTankMoveSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdTankMoveAck(Player* player, JNetwork::ICommand* cmd);
};
