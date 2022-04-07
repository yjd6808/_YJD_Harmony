#include <TF/Scenes/SynchronizedScene.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>


/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/

void SynchronizedScene::CmdServerMessageSyn(ICommand* cmd) {
	ServerMessageSyn* pServerMessageSyn = cmd->CastCommand<ServerMessageSyn*>();
	PopUp::createInParent(pServerMessageSyn->Message, this, false);
}

void SynchronizedScene::SynchronizedOnReceived(ICommand* cmd) {
	switch (cmd->GetCommand()) {
	case SERVER_MESSAGE_SYN: this->CmdServerMessageSyn(cmd); break;
	}
}
