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

	// �濡 ������ �������� ��û�Ѵ�.
	SendFn::SendLoadRoomSyn();
}

/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/