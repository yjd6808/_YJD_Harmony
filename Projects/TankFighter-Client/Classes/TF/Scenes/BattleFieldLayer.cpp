#include <TF/Scenes/BattleFieldLayer.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>


void BattleFieldLayer::onEnterTransitionDidFinish() {
	// ��Ʋ�ʵ忡 ������ 
	// SendFn::SendLoadCharacterInfoSyn();
}

bool BattleFieldLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	
	
}


/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/
