/*
 * �ۼ��� : ������
 */

#include <TF/Scenes/ChannelLayer.h>
#include <TF/Object/Obstacle.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/UI/ColoredListView.h>
#include <TF/Network/GameClient.h>
#include <TF/Network/SendFn.h>


void ChannelLayer::onEnterTransitionDidFinish() {

	// ä�ξ��� �����ϸ� ä�� ������ �ޱ����� �ٷ� ��Ŷ�� ������.
	if (SendFn::SendLoadChannelInfoSyn() == false) {
		PopUp::createInParent("ä�� ���� ��û�� �����Ͽ����ϴ�.", this, false);
	}
}


bool ChannelLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	

	return true;
}

void ChannelLayer::OnChannelClick(TextButton* textButton) {
	SendFn::SendChannelSelectSyn(textButton->getTag());	// ��ư �±װ� ä�� UID�� CmdLoadChannelInfoAck() ����
}




/* =================================================================================
 *
 *                             ��� ��Ŷ ó�� 
 *	
 * ================================================================================*/


void ChannelLayer::CmdLoadChannelInfoAck(ICommand* cmd) {
	LoadChannelInfoAck* pChannelInfoAck = cmd->CastCommand<LoadChannelInfoAck*>();

	for (int i = 0; i < pChannelInfoAck->Count; i++) {
		ChannelInfo* pChannelInfo = &pChannelInfoAck->Info[i];

		TextButton* btn = TextButton::create(200, 45, StringUtils::format("%s (%d/%d)", pChannelInfo->Name, pChannelInfo->PlayerCount, pChannelInfo->MaxPlayerCount), 14);
		btn->setAnchorPoint(Vec2::ZERO);
		btn->setPosition({ 400, float(350 - 50 * i )});
		btn->setColor(ColorList::Ashgray_v);
		btn->setFontColor(ColorList::Black_v);
		btn->setClickEvent(CC_CALLBACK_1(ChannelLayer::OnChannelClick, this));
		this->addChild(btn, 0, pChannelInfo->ChannelUID);		// �±׷� ä�� UID�� ��������.
	}

	PopUp::createInParent("���� ����!", this, false);
}

void ChannelLayer::CmdSelectChannelAck(ICommand* cmd) {
	const SelectChannelAck* pChannelInfoAck = cmd->CastCommand<SelectChannelAck*>();

	if (pChannelInfoAck->Result) {
		// ä�� CharacterUID ����
		_Client->SetChannelUID(pChannelInfoAck->ChanneldUID);
		_Client->ChangeScene(SceneType::CharacterSelect);
		return;
	}

	PopUp::createInParent(pChannelInfoAck->Reason, this, false);
}
