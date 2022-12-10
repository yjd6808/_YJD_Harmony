/*
 * Cocos2d-x & Socket Programming 샘플 참고좀함
 * @참고 : https://github.com/AlexanderShniperson/cocos2dx_socket_example
 *  
 */

#include <TF/Network/GameClientEventListener.h>
#include <TF/Scenes/SynchronizedScene.h>
#include <TF/AppDelegate.h>

#include <TF/Scenes/LoginScene.h>
#include <TF/UI/PopUp.h>

#include <JCore/RAII/AutoPtr.h>

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
	// 서버 강제로 끊기면 여기서 릭 생길 수 잇는데..
	// 결국 Command Lock 필요하겠네.. 수정해야할 것같다.
	// 완성하고 시간남으면 ㄱ
	// => 수정완료

	{
		NormalLockGuard guard(m_CommandQueueMtx);

		// 이게 내가 선택지가 2개다.
		// 1. 강종됬을 때 큐에 커맨드들이 있을 때 이게 모두 처리될때까지 기다리든지
		// 2. 처리하지 않고 그냥 큐에 담긴 모든 커맨드들을 삭제한다. (이걸로 함)
		while (!m_CommandQueue.IsEmpty()) {
			char* pCmd = m_CommandQueue.Front();
			m_CommandQueue.Dequeue();

			DeleteArraySafe(pCmd);
		}
	}

	Director::getInstance()->getScheduler()->performFunctionInCocosThread(
		CC_CALLBACK_0(GameClientEventListener::SynchronizedOnDisconnected, this)
	);
	CCLOG("Client Disconnected");
}

void GameClientEventListener::OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {
	// Overalapped Send 결과

	Int16U* cmd = reinterpret_cast<Int16U*>(sentPacket->GetWSABuf().buf + PACKET_HEADER_SIZE);

	// 움직이는거 업데이트는 출력안함
	// RTT 핑 패킷 전송 출력안함
	if ((*cmd != BATTLE_FIELD_TANK_MOVE_SYN) && (*cmd != TCP_RTT_SYN)) {
		CCLOG("%d 커맨드 전송", *cmd);
	}
}

/*
 * IOCP 쓰레드에서 돌아감
 * Cocos 씬에서 처리하기 위해 Cocos 쓰레드와 동기화 필요
 */
void GameClientEventListener::OnReceived(ICommand* cmd) {

	// 커맨드는 클라이언트의 ReceiveBuffer의 일부로 사용중이므로
	// 코코스 쓰레드에서 받는 중에 해당 메모리가 덮어쒸워질 우려가 있다.
	// 락을 사용해주거나 동적할당한 데이터를 넘겨주는 식으로 처리해야할 것으로보인다.
	int iNewAllocCapacity = cmd->GetCommandLen();
	char* pNewAlloc = new char[iNewAllocCapacity];
	Memory::Copy(pNewAlloc, iNewAllocCapacity, cmd, iNewAllocCapacity);

	// 커맨드를 전달할 방법이 없으므로 큐에 담아서 동기화된 쓰레드에서 빼도록 해준다.

	{
		NormalLockGuard guard(m_CommandQueueMtx);
		m_CommandQueue.Enqueue(pNewAlloc);
	}
	
	// 인자를 전달할 수 없는 std::functin<void()> 타입이므로 동기화 큐에 커맨드를 넣어준 후 빼주는 식으로 해야한다.
	Director::getInstance()->getScheduler()->performFunctionInCocosThread(
		CC_CALLBACK_0(GameClientEventListener::SynchronizedOnReceived, this)
	);
}


/*
 * Cocos 쓰레드에서 돌아감
 */
void GameClientEventListener::SynchronizedOnReceived() {
	char* pNewAlloc = nullptr;

	{
		NormalLockGuard guard(m_CommandQueueMtx);

		// 함수 실행중 갑자기 서버가 닫혀서 커맨드 큐가 모두 비어버리게 될 수 있다.
		if (m_CommandQueue.IsEmpty())
			return;

		pNewAlloc = m_CommandQueue.Front();
		m_CommandQueue.Dequeue();
	}

	// 명령 수행 후 삭제를 해주도록 하자.
	AutoPtr<char> autoDelete(pNewAlloc, Deletor<char[]>());		
	ICommand* pCmd = reinterpret_cast<ICommand*>(pNewAlloc);

	// 현재 실행중인 씬을 가져온다.
	Scene* runningScene = Director::getInstance()->getRunningScene();

	if (runningScene == nullptr) {
		CCLOG("현재 동작중인 씬이 없습니다.");
		return;
	}

	SynchronizedScene* synchronizedScene = dynamic_cast<SynchronizedScene*>(runningScene);

	if (synchronizedScene == nullptr) {
		// 참고 코드보면 TransitionScene이 타입이 있던데.. 그건 씬 전환할때 애니메이션이 있으면 잠깐 생성되는 그런건가?
		// 내가 구현한 씬은 모두 SynchronizedScene을 상속받아 구현하기 땜에 여기 들어오면 안댈 듯?
		CCLOG("동기화 씬이 nullptr입니다. 씬 타입을 확인해주세요.");
		return;
	}

	if (synchronizedScene != nullptr)
		synchronizedScene->SynchronizedOnReceived(pCmd);
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
		PopUp::createInParent("서버와 연결이 끊어졌습니다.", pLoginScene, false);
	}
}

void GameClientEventListener::SynchronizedOnConnected() {
	Scene* runningScene = Director::getInstance()->getRunningScene();

	if (runningScene)
		PopUp::createInParent("서버에 성공적으로 접속하였습니다.", runningScene, false);
}
