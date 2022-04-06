#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>



bool BattleFieldLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	// 배틀필드에 들어오면 
	SendFn::SendLoadCharacterInfoSyn();
	
}

/* =================================================================================
 *
 *                             통신 패킷 처리
 *
 * ================================================================================*/