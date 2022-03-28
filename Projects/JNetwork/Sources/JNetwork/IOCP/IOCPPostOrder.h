/*
	�ۼ��� : ������

	IOCP ��Ŀ�����忡 PostQueuedCompletionStatus�� CompletionKey������ �������� ��ü
	Pause Resume Join�� ���� ����� ���� �� �����
*/

#pragma once

#include <Windows.h>

namespace JNetwork {

class IOCPWorker;
struct IOCPPostOrder
{
	int Order = 0;
	HANDLE Handle = nullptr;

	// Order�� �״�� ��ȯ����
	// �������� ó���� �����ϰ� ���� Worker�����忡�� �� ��ȯ���� Ȯ���Ͽ� ��� �������� �ƴϸ� �������� ���� ���θ� �����ϵ��� �Ѵ�.
	int Process(IOCPWorker* worker);	
	void Release() { delete this; }
};

} // namespace JNetwork