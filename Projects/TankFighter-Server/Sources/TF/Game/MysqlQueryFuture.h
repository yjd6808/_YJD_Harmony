/*
	�ۼ��� : ������
	IOCP�� Ȱ���� �񵿱� ���� ó���� �����ϵ��� ������.
*/

#pragma once

#include <TF/Database/MysqlQuery.h>
#include <JCore/SmartPointer.h>
#include <JCore/RefCount.h>
#include <Windows.h>

class MysqlQueryFuture
{
private:
	MysqlQueryFuture()
		: m_WaitHandle(CreateEventA(NULL, TRUE, TRUE, NULL)) {

		if (m_WaitHandle == INVALID_HANDLE_VALUE)
			DebugAssert(false, "�ڵ��� ����� �������� �ʾҽ��ϴ�.");
	}
public:
	~MysqlQueryFuture() {
		if (m_WaitHandle != INVALID_HANDLE_VALUE)
			CloseHandle(m_WaitHandle);
	}
public:
	JCore::SharedPointer<MysqlQuery> Wait() const {
		DebugAssert(m_WaitHandle != INVALID_HANDLE_VALUE, "�ڵ��� nullptr �Դϴ�.");

		ResetEvent(m_WaitHandle);	
		WaitForSingleObject(m_WaitHandle, INFINITE);
	}

	template <typename... Args>
	static JCore::UniquePointer<MysqlQueryFuture> ExecuteAsync(const std::string& statement, Args&&... args) {


		return JCore::MakeUnique<MysqlQueryFuture>();
	}
private:
	HANDLE m_WaitHandle = INVALID_HANDLE_VALUE;
	JCore::SharedPointer<MysqlQuery> m_spResult = nullptr;
};
