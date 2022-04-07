/*
 * Cocos2d-x & Socket Programming ���� ��������
 * @���� : https://github.com/AlexanderShniperson/cocos2dx_socket_example
 *  
 */

#define _WINSOCKAPI_

#include <TF/Network/GameClientEventListener.h>
#include <TF/Scenes/SynchronizedScene.h>
#include <TF/AppDelegate.h>

#include <TF/Scenes/LoginScene.h>
#include <TF/UI/PopUp.h>

#include <cocos2d.h>

using namespace JCore;
using namespace JNetwork;
using namespace cocos2d;


void GameClientEventListener::OnConnected() {
	CCLOG("Client Connected");
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(
		CC_CALLBACK_0(GameClientEventListener::SynchronizedOnConnected, this)
	);
}

void GameClientEventListener::OnDisconnected() {
	m_CommandQueue.Clear();
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(
		CC_CALLBACK_0(GameClientEventListener::SynchronizedOnDisconnected, this)
	);
	CCLOG("Client Disconnected");
}

void GameClientEventListener::OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
	// Overalapped Send ���
	CCLOG("��Ŷ ����");
}

/*
 * IOCP �����忡�� ���ư�
 * Cocos ������ ó���ϱ� ���� Cocos ������� ����ȭ �ʿ�
 */
void GameClientEventListener::OnReceived(ICommand* cmd) {

	// Ŀ�ǵ�� Ŭ���̾�Ʈ�� ReceiveBuffer�� �Ϻη� ������̹Ƿ�
	// ���ڽ� �����忡�� �޴� �߿� �ش� �޸𸮰� ������� ����� �ִ�.
	// ���� ������ְų� �����Ҵ��� �����͸� �Ѱ��ִ� ������ ó���ؾ��� �����κ��δ�.
	// m_CommandQueueMtx.Lock();
	int iNewAllocCapacity = cmd->GetCommandLen();
	char* pNewAlloc = new char[iNewAllocCapacity];
	Memory::Copy(pNewAlloc, iNewAllocCapacity, cmd, iNewAllocCapacity);

	// Ŀ�ǵ带 ������ ����� �����Ƿ� ť�� ��Ƽ� ����ȭ�� �����忡�� ������ ���ش�.
	m_CommandQueue.Enqueue(pNewAlloc);

	// ���ڸ� ������ �� ���� std::functin<void()> Ÿ���̹Ƿ� ����ȭ ť�� Ŀ�ǵ带 �־��� �� ���ִ� ������ �ؾ��Ѵ�.
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(
		CC_CALLBACK_0(GameClientEventListener::SynchronizedOnReceived, this)
	);

}


/*
 * Cocos �����忡�� ���ư�
 */
void GameClientEventListener::SynchronizedOnReceived() {
	// m_CommandQueueMtx.Unlock();
	char* pNewAlloc =  m_CommandQueue.Front();
	ICommand* pCmd = reinterpret_cast<ICommand*>(pNewAlloc);
	m_CommandQueue.Dequeue();

	// ���� �������� ���� �����´�.
	Scene* runningScene = Director::getInstance()->getRunningScene();

	if (runningScene == nullptr) {
		CCLOG("���� �������� ���� �����ϴ�.");
		return;
	}

	SynchronizedScene* synchronizedScene = dynamic_cast<SynchronizedScene*>(runningScene);

	if (synchronizedScene == nullptr) {
		// ���� �ڵ庸�� TransitionScene�� Ÿ���� �ִ���.. �װ� �� ��ȯ�Ҷ� �ִϸ��̼��� ������ ��� �����Ǵ� �׷��ǰ�?
		// ���� ������ ���� ��� SynchronizedScene�� ��ӹ޾� �����ϱ� ���� ���� ������ �ȴ� ��?
		CCLOG("����ȭ ���� nullptr�Դϴ�. �� Ÿ���� Ȯ�����ּ���.");
		return;
	}

	if (synchronizedScene != nullptr)
		synchronizedScene->SynchronizedOnReceived(pCmd);


	// ����� �����ϰ� �������ֵ��� ����.
	DeleteArraySafe(pNewAlloc);
}

void GameClientEventListener::SynchronizedOnDisconnected() {
	Scene* runningScene = Director::getInstance()->getRunningScene();

	for (const auto& child : runningScene->getChildren()) {
		child->unscheduleUpdate();
		child->getEventDispatcher()->removeAllEventListeners();
	}

	Scene* pScene = LoginScene::createScene();
	Director::getInstance()->replaceScene(pScene);

	LoginScene* pLoginScene = dynamic_cast<LoginScene*>(pScene);

	if (pLoginScene) {
		PopUp::createInParent("������ ������ ���������ϴ�.", pLoginScene, false);
	}
}

void GameClientEventListener::SynchronizedOnConnected() {
	Scene* runningScene = Director::getInstance()->getRunningScene();

	if (runningScene)
		PopUp::createInParent("������ ���������� �����Ͽ����ϴ�.", runningScene, false);
}
