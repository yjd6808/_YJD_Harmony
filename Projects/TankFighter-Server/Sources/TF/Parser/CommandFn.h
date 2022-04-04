#pragma once

class Player;

namespace JNetwork {
	struct ICommand;
	class TcpSession;
}

class CommandFn
{
public:
	static void CmdLoginSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdRegisterSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdLoadChannelInfoSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdSelectChannelSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdLoadCharacterInfoSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdSelectCharacterSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdCreateCharacterSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdDeleteCharacterSyn(Player* player, JNetwork::ICommand* cmd);

	static void CmdTankMoveSyn(Player* player, JNetwork::ICommand* cmd);
	static void CmdTankMoveAck(Player* player, JNetwork::ICommand* cmd);
};
