/**
 *  �ۼ��� : ������
 *	����ȭ�� �����忡�� ��Ŷ ����
 *  �����ϴ� ��� ���� �̳༮�� ��ӹ޵��� �սô�.
 *	������ ��Ŷ�� ó���ؾ��ϴϱ�..
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
