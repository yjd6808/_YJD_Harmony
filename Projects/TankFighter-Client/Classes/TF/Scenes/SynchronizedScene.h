/**
 *  작성자 : 윤정도
 *	동기화된 쓰레드에서 패킷 수신
 *  개발하는 모든 씬은 이녀석을 상속받도록 합시다.
 *	수신한 패킷을 처리해야하니까..
 *
 *	그리고 모든 씬에서 공통적으로 처리해야하는 패킷을 처리하도록 합니다.
 */

#pragma once

#include <TF/SourceHeader.h>

namespace JNetwork
{
	struct ICommand;
}

class SynchronizedScene : public Scene
{

	void CmdServerMessageSyn(ICommand* cmd);
public:
	virtual void SynchronizedOnReceived(ICommand* cmd);
};
