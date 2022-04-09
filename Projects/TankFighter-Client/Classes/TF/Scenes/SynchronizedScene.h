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

namespace JNetwork
{
	struct ICommand;
}

class SynchronizedScene : public Scene
{
protected:
	bool init() override;
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void CmdServerMessageSyn(ICommand* cmd);
public:
	virtual void SynchronizedOnReceived(ICommand* cmd);
protected:
	GridLayer* m_pGridLayer;
};
