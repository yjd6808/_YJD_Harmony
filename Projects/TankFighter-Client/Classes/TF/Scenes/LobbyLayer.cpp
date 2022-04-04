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

	// 채널씬에 진입하면 채널 정보를 받기위해 바로 패킷을 보낸다.
	SendFn::SendLoadChannelInfoSyn();

	return true;
}



/* =================================================================================
 *
 *                             통신 패킷 처리 
 *	
 * ================================================================================*/



void LobbyLayer::CmdLoadChannelInfoAck(ICommand* cmd) {
	LoadChannelInfoAck* pChannelInfoAck = cmd->CastCommand<LoadChannelInfoAck*>();

	
}
