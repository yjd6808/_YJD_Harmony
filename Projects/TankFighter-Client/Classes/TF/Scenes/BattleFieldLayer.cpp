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

	// ��Ʋ�ʵ忡 ������ 
	SendFn::SendLoadCharacterInfoSyn();
	
}

/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/