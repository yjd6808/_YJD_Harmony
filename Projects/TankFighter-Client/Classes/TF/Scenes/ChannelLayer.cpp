/*
 * 작성자 : 윤정도
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

	// 채널씬에 진입하면 채널 정보를 받기위해 바로 패킷을 보낸다.
	if (SendFn::SendLoadChannelInfoSyn() == false) {
		PopUp::createInParent("채널 정보 요청이 실패하였습니다.", this, false);
	}
}


bool ChannelLayer::init() {
	if (!Layer::init()) {
		return false;
	}

	

	return true;
}

void ChannelLayer::OnChannelClick(TextButton* textButton) {
	SendFn::SendChannelSelectSyn(textButton->getTag());	// 버튼 태그가 채널 UID임 CmdLoadChannelInfoAck() 참고
}




/* =================================================================================
 *
 *                             통신 패킷 처리 
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
		this->addChild(btn, 0, pChannelInfo->ChannelUID);		// 태그로 채널 UID를 지정하자.
	}

	PopUp::createInParent("접속 성공!", this, false);
}

void ChannelLayer::CmdSelectChannelAck(ICommand* cmd) {
	const SelectChannelAck* pChannelInfoAck = cmd->CastCommand<SelectChannelAck*>();

	if (pChannelInfoAck->Result) {
		// 채널 CharacterUID 설정
		_Client->SetChannelUID(pChannelInfoAck->ChanneldUID);
		_Client->ChangeScene(SceneType::CharacterSelect);
		return;
	}

	PopUp::createInParent(pChannelInfoAck->Reason, this, false);
}
