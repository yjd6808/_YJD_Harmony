#include <TF/Scenes/RoomLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>



bool RoomLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// 방에 들어오면 방정보를 요청한다.
	SendFn::SendLoadRoomSyn();
}

/* =================================================================================
 *
 *                             통신 패킷 처리
 *
 * ================================================================================*/