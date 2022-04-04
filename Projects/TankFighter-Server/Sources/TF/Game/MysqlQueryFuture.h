/*
	작성자 : 윤정도
	IOCP를 활용한 비동기 쿼리 처리를 지원하도록 만들자.
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
			DebugAssert(false, "핸들이 제대로 생성되지 않았습니다.");
	}
public:
	~MysqlQueryFuture() {
		if (m_WaitHandle != INVALID_HANDLE_VALUE)
			CloseHandle(m_WaitHandle);
	}
public:
	JCore::SharedPointer<MysqlQuery> Wait() const {
		DebugAssert(m_WaitHandle != INVALID_HANDLE_VALUE, "핸들이 nullptr 입니다.");

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
