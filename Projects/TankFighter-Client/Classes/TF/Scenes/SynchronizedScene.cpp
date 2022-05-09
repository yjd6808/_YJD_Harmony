/**
 *  �ۼ��� : ������
 *	����ȭ�� �����忡�� ��Ŷ ����
 *  �����ϴ� ��� ���� �̳༮�� ��ӹ޵��� �սô�.
 *	������ ��Ŷ�� ó���ؾ��ϴϱ�..
 */

#include <TF/Scenes/SynchronizedScene.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>
#include <TF/UI/PopUp.h>
#include <TF/Network/SendFn.h>
#include <TF/Network/ClientConfiguration.h>

#include <JCore/Ascii.h>


bool SynchronizedScene::init() {

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SynchronizedScene::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	m_pGridLayer->setAnchorPoint(Vec2::ZERO);
	m_pGridLayer->setVisible(false);
	this->addChild(m_pGridLayer, 99999);
	

	m_pNetInfo = Text::create("fdsfdsfspfmjsdopdjsmivosdjfoisjfiosjfdosi;fjdsio;fdjs;oifsjiof;sdjfio;sdjfi;odsjfdsi;lfjdsli;fdsjlf;ji", FONT_PATH_DEFAULT, 13);
	m_pNetInfo->setVisible(false);
	m_pNetInfo->setColor(ColorList::Arcticlime_v);
	m_pNetInfo->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	m_pNetInfo->setPosition({ 0, MAP_HEIGHT });
	this->addChild(m_pNetInfo, 43434);
	this->scheduleUpdate();


	return Scene::init();
}

// ESC ������ ������ �پ��ϰ� �����ְ� �� ���ؽ� ���� �����ֵ��� ��
void SynchronizedScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		Director::getInstance()->setDisplayStats(!Director::getInstance()->isDisplayStats());
		m_pGridLayer->setVisible(!m_pGridLayer->isVisible());
		m_pNetInfo->setVisible(!m_pGridLayer->isVisible());
		break;
	}
}


// 0.5�ʸ��� ������
void SynchronizedScene::update(float delta) {
	m_fPingDelay += delta;

	if (m_fPingDelay >= PING_DELAY) {
		auto pPacket = new StaticPacket<TcpRTTSyn>;
		TcpRTTSyn* pTcpRTTSyn = pPacket->Get<0>();
		m_previousSentTime = DateTime::Now();
		pTcpRTTSyn->Tick = m_previousSentTime.GetTick();
		_Client->SendAsync(pPacket);
		m_fPingDelay = 0.0f;
	}
}


/* =================================================================================
 *
 *                             ��� ��Ŷ ó��
 *
 * ================================================================================*/


void SynchronizedScene::CmdServerMessageSyn(ICommand* cmd) {
	ServerMessageSyn* pServerMessageSyn = cmd->CastCommand<ServerMessageSyn*>();
	PopUp::createInParent(pServerMessageSyn->Message, this, false);
}

void SynchronizedScene::CmdTcpRTTAck(ICommand* cmd) {
	TcpRTTAck* pTcpRTTAck = cmd->CastCommand<TcpRTTAck*>();

	DateTime now = DateTime::Now();
	m_Latency = DateTime{ static_cast<Int64>(pTcpRTTAck->Tick) }.Diff(m_previousSentTime);
	m_RTT = now.Diff(m_previousSentTime);

	JCore::String szNetInfo(250);
	szNetInfo += "Latency : ";
	szNetInfo += m_Latency.GetMiliSecond();
	szNetInfo += "ms\n";
	szNetInfo += "Round Trip Time : ";
	szNetInfo += m_RTT.GetMiliSecond();
	szNetInfo += "ms\n";
	szNetInfo += "Remote IP : ";
	szNetInfo += _Client->GetRemoteEndPoint().ToString() + "\n";
	szNetInfo += "Bind Port : ";
	szNetInfo += _Client->GetLocalEndPoint().GetPort();

	if (m_pNetInfo) {
		m_pNetInfo->setText(szNetInfo.Source());
		m_pNetInfo->setVisible(true);
	}
}

// false ��ȯ�ϴ°�� Derived ������ ó���� ����
bool SynchronizedScene::SynchronizedOnReceived(ICommand* cmd) {
	switch (cmd->GetCommand()) {
	case SERVER_MESSAGE_SYN: 
		this->CmdServerMessageSyn(cmd);
		return false;
	case TCP_RTT_ACK:		 
		this->CmdTcpRTTAck(cmd);
		return false;
	}

	return true;
}
