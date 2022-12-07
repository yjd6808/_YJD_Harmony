/*
	작성자 : 윤정도

	IOCP 워커쓰레드에 PostQueuedCompletionStatus의 CompletionKey값으로 전달해줄 객체
	Pause Resume Join과 같은 명령을 보낼 때 사용함
*/

#pragma once

#include <JCore/Type.h>

namespace JNetwork {

class IOCPWorker;
struct IOCPPostOrder
{
	int Order = 0;
	WinHandle Handle = nullptr;

	// Order값 그대로 반환해줌
	// 세부적인 처리를 진행하고 이후 Worker쓰레드에서 이 반환값을 확인하여 계속 진행할지 아니면 종료할지 등의 여부를 결정하도록 한다.
	int Process(IOCPWorker* worker);	
	void Release() const { delete this; }
};

} // namespace JNetwork