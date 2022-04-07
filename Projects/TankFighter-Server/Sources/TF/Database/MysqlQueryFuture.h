/*
	�ۼ��� : ������
	IOCP�� Ȱ���� �񵿱� ���� ó���� �����ϵ��� ������.
	����� ���ϸ� �޾ƺ� �� �ֵ�����
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
	// �Ʒ� 4�� �Լ����� ��� IOCPOverlappedQuery ���� ����
	void InitializeQuery(MysqlConnection* conn);
	void ExecuteQuery();
	void SetReady() { m_bReady = true; }
	void Signal() const { SetEvent(m_WaitHandle); }
	void CallbackExecute();
private:
	HANDLE m_WaitHandle;
	std::atomic<bool> m_bReady;
	JCore::String m_Statement;		// ������ ������ ����� ���ķδ� ��� �Ұ����ϴ� ���� (move�� MysqlQuery�� �Űܼ� ���Ŷ�)
	StatementType m_eStatement;
	union { MysqlQuery m_Result; };	// IOCPOverlappedQuery�� Process �Լ����� �ʱ�ȭ�� �ǵ��� �ϱ� ���� ������ union���� ������ ȣ���� ���´�. (���� IOCP ������ ���ϸ� �����̳��� �����ͺ��̽� IOCP ������� �ű�� ����)
	JCore::Action<MysqlQuery*> m_Callback;
	
	friend class IOCPOverlappedQuery;
};


