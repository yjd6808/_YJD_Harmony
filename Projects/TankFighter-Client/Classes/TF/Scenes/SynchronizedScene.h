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
