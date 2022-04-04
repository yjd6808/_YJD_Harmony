#include <TF/Scenes/LobbyLayer.h>
#include <TF/Object/Obstacle.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/UI/ColoredListView.h>
#include <TF/Network/GameClient.h>
#include <TF/Network/SendFn.h>

using namespace JNetwork;

bool LobbyLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// ä�ξ��� �����ϸ� ä�� ������ �ޱ����� �ٷ� ��Ŷ�� ������.
	SendFn::SendLoadChannelInfoSyn();

	return true;
}



/* =================================================================================
 *
 *                             ��� ��Ŷ ó�� 
 *	
 * ================================================================================*/



void LobbyLayer::CmdLoadChannelInfoAck(ICommand* cmd) {
	LoadChannelInfoAck* pChannelInfoAck = cmd->CastCommand<LoadChannelInfoAck*>();

	
}
