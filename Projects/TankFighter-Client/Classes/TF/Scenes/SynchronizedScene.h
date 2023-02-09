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
#include <TF/Scenes/GridLayer.h>

#include <JCore/Time.h>

namespace JNetwork
{
	struct ICommand;
}

class SynchronizedScene : public Scene
{
protected:
	bool init() override;
	void onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void update(float delta) override;


	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdServerMessageSyn(ICommand* cmd);
	void CmdTcpRTTAck(ICommand* cmd);
public:
	
	virtual bool SynchronizedOnReceived(ICommand* cmd);
protected:
	float m_fPingDelay = 0.0f;

	TimeSpan m_Latency;
	TimeSpan m_RTT;
	DateTime m_previousSentTime;
	GridLayer* m_pGridLayer;
	Text* m_pNetInfo;
};
