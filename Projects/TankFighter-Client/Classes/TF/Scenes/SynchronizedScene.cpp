/**
 *  작성자 : 윤정도
 *	동기화된 쓰레드에서 패킷 수신
 *  개발하는 모든 씬은 이녀석을 상속받도록 합시다.
 *	수신한 패킷을 처리해야하니까..
 */

#include <TF/Scenes/SynchronizedScene.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>

#include <JCore/Ascii.h>


bool SynchronizedScene::init() {

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SynchronizedScene::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	m_pGridLayer->setAnchorPoint(Vec2::ZERO);
	m_pGridLayer->setVisible(false);
	this->addChild(m_pGridLayer);

	return Scene::init();
}

// ESC 누르면 정보들 다양하게 보여주게 함 벌텍스 정보 보여주도록 함
void SynchronizedScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		Director::getInstance()->setDisplayStats(!Director::getInstance()->isDisplayStats());
		m_pGridLayer->setVisible(!m_pGridLayer->isVisible());
		break;
	}
}




/* =================================================================================
 *
 *                             통신 패킷 처리
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
