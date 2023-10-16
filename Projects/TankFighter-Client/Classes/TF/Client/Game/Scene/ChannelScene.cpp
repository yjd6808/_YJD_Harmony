/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "ChannelScene.h"

#include <TF/Common/Structure.h>
#include <TF/Client/Net/Send/S_GAME.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

ChannelScene::ChannelScene()
{}

ChannelScene::~ChannelScene()
{}

void ChannelScene::onEnterTransitionDidFinish() {
	BaseScene::onEnterTransitionDidFinish();

	// 채널씬에 진입하면 채널 정보를 받기위해 바로 패킷을 보낸다.
	if (S_GAME::SEND_CS_LoadChannelInfo() == false) {
		PopUp::createInParent("채널 정보 요청이 실패하였습니다.", this, false);
	}
}

bool ChannelScene::init() {

	if (!BaseScene::init()) {
		return false;
	}

	this->scheduleUpdate();
	return true;
}

void ChannelScene::refreshChannelList(ChannelInfo* channelList, int count) {
	m_pUILayer->removeAllChildren();

	for (int i = 0; i < count; i++) {
		ChannelInfo& channelInfo = channelList[i];

		TextButton* btn = TextButton::create(200, 45, StringUtils::format("%s (%d/%d)", channelInfo.Name.Source, channelInfo.PlayerCount, channelInfo.MaxPlayerCount), 14);
		btn->setAnchorPoint(Vec2::ZERO);
		btn->setPosition({ 400, float(350 - 50 * i) });
		btn->setBackgroundColor(ColorList::Ashgray_v);
		btn->setFontColor(ColorList::Black_v);
		btn->setClickEvent(CC_CALLBACK_1(ChannelScene::onChannelBtnClick, this));
		m_pUILayer->addChild(btn, 0, channelInfo.PrimaryKey);						// 태그로 채널 UID를 지정하자.
	}
}

void ChannelScene::onChannelBtnClick(TextButton* textButton) {
	S_GAME::SEND_CS_SelectChannel(textButton->getTag()); // 버튼 태그가 채널 UID임 CmdLoadChannelInfoAck() 참고
}

