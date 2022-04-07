/*
	작성자 : 윤정도
	IOCP를 활용한 비동기 쿼리 처리를 지원하도록 만들자.
	결과를 원하면 받아볼 수 있도록함
*/


#pragma once


#include <TF/Database/MysqlQuery.h>
#include <TF/Database/StatementType.h>
#include <JCore/RefCount.h>
#include <JCore/Functional.h>
#include <Windows.h>

// ReSharper disable CppPossiblyUninitializedMember

class MysqlQueryFuture : public JCore::RefCount
{
public:
	MysqlQueryFuture(const JCore::String& statement);
	~MysqlQueryFuture() override;
public:
	MysqlQuery& Wait();
private:	
	// 아래 4종 함수들은 모두 IOCPOverlappedQuery 에서 사용됨
	void InitializeQuery(MysqlConnection* conn);
	void ExecuteQuery();
	void SetReady() { m_bReady = true; }
	void Signal() const { SetEvent(m_WaitHandle); }
	void CallbackExecute();
private:
	HANDLE m_WaitHandle;
	std::atomic<bool> m_bReady;
	JCore::String m_Statement;		// 쓰고나면 쿼리가 실행된 이후로는 사용 불가능하니 주의 (move로 MysqlQuery로 옮겨서 쓸거라서)
	StatementType m_eStatement;
	union { MysqlQuery m_Result; };	// IOCPOverlappedQuery의 Process 함수에서 초기화가 되도록 하기 위해 생성자 union으로 생성자 호출을 막는다. (서버 IOCP 쓰레드 부하를 조금이나마 데이터베이스 IOCP 쓰레드로 옮기기 위함)
	JCore::Action<MysqlQuery*> m_Callback;
	
	friend class IOCPOverlappedQuery;
};


