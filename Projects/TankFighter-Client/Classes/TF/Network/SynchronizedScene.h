/**
 *  작성자 : 윤정도
 *	동기화된 쓰레드에서 패킷 수신
 *  개발하는 모든 씬은 이녀석을 상속받도록 합시다.
 *	수신한 패킷을 처리해야하니까..
 */

#pragma once

#include <TF/SourceHeader.h>

namespace JNetwork
{
	struct ICommand;
}

class SynchronizedScene : public Scene
{
public:
	virtual void SynchronizedOnReceived(JNetwork::ICommand* cmd) = 0;
};
