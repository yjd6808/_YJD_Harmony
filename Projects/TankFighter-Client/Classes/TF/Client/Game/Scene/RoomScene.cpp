/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:29 PM
 * =====================
 *
 */

#include "Pch.h"
#include "RoomScene.h"

#include <TF/Common/Structure.h>
#include <TF/Client/Net/Send/S_GAME.h>
#include <TF/Client/Game/Scene/ChannelScene.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_CCUI;

RoomScene::RoomScene()
{}

RoomScene::~RoomScene()
{}

void RoomScene::onEnterTransitionDidFinish() {
	BaseScene::onEnterTransitionDidFinish();

	if (S_GAME::SEND_CS_LoadRoomInfo(Core::GameClient->GetRoomAccessId()) == false) {
		PopUp::createInParent("방 정보 요청이 실패하였습니다.", this, false);
	}
}


bool RoomScene::init() {

	if (!BaseScene::init()) {
		return false;
	}


	this->scheduleUpdate();
	return true;
}


void RoomScene::refreshRoomInfo(RoomInfo& info) {
}

void RoomScene::refreshRoomMemberInfoList(RoomCharacterInfo* roomCharacterInfoList, int count, int hostCharacterPrimaryKey) {

}
