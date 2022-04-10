/**
 *  �ۼ��� : ������
 *	����ȭ�� �����忡�� ��Ŷ ����
 *  �����ϴ� ��� ���� �̳༮�� ��ӹ޵��� �սô�.
 *	������ ��Ŷ�� ó���ؾ��ϴϱ�..
 *
 *	�׸��� ��� ������ ���������� ó���ؾ��ϴ� ��Ŷ�� ó���ϵ��� �մϴ�.
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
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void update(float delta) override;


	/* =================================================================================
	 *                             ��� ��Ŷ ó��
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
