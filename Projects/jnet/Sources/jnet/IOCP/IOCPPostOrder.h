/*
	작성자 : 윤정도

	IOCP 워커쓰레드에 PostQueuedCompletionStatus의 CompletionKey값으로 전달해줄 객체
	Pause Resume Join과 같은 명령을 보낼 때 사용함
*/

#pragma once

#include <jc/Sync/WaitHandle.h>
#include <jc/Primitives/RefCountObject.h>

#include <jnet/Namespace.h>

NS_JNET_BEGIN

class IOCPWorker;

struct IOCPPostOrder : jc::RefCountObject
{
	int order_ = 0;
	jc::WaitHandle* waitHandle_ = nullptr;

	// Order값 그대로 반환해줌
	// 세부적인 처리를 진행하고 이후 Worker쓰레드에서 이 반환값을 확인하여 계속 진행할지 아니면 종료할지 등의 여부를 결정하도록 한다.
	int Process(IOCPWorker* _pWorker);
	void ReleaseAction() override { delete this; }
};

NS_END
