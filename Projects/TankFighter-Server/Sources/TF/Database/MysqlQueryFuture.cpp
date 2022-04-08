#include <TF/PrecompiledHeader.h>
#include <TF/Database/MysqlQueryFuture.h>

#include <JCore/Memory.h>
#include <algorithm>

using namespace JCore;

MysqlQueryFuture::MysqlQueryFuture(const JCore::String& statement) :
		m_WaitHandle(CreateEventA(NULL, TRUE, FALSE, NULL)),
		m_Statement(statement)
{
	m_eStatement = MysqlQuery::ParseStatement(statement);
	if (m_WaitHandle == INVALID_HANDLE_VALUE)
		DebugAssert(false, "�ڵ��� ����� �������� �ʾҽ��ϴ�.");
}

MysqlQueryFuture::~MysqlQueryFuture() {
	if (m_WaitHandle != INVALID_HANDLE_VALUE)
		CloseHandle(m_WaitHandle);

	// ���⼭ ���� �����ָ� MysqlQuery�̳༮ ũ�⸸ŭ �����Ҵ� ������ �ȵȴ�. �׷��� �������� ȣ��������� / 22/04/07 ���� 8��
	Memory::PlacementDeallocate(m_Result);
}

MysqlQuery& MysqlQueryFuture::Wait() {
	DebugAssert(m_WaitHandle != INVALID_HANDLE_VALUE, "�ڵ��� nullptr �Դϴ�.");
	if (m_bReady == false) {
		// IOCPOverlappedQuery���� ���� ���� ����� �غ� �ȵ� ��� �ñ׳��� ���������� ��ٸ���.
		WaitForSingleObject(m_WaitHandle, INFINITE);
	}

	return m_Result;
}

void MysqlQueryFuture::InitializeQuery(MysqlConnection* conn) {
	Memory::PlacementAllocate(m_Result, conn, m_Statement);
}

void MysqlQueryFuture::ExecuteQuery() {
	switch (m_eStatement) {
	case StatementType::Select:
		m_Result.ExecuteQuery();
		break;
	case StatementType::Insert:
		m_Result.ExecuteInsert();
		break;
	case StatementType::Update:
	case StatementType::Delete:
		m_Result.ExecuteUpdate();
		break;
	default:
		m_Result.ExecuteQuery();
		break;
	}
}

void MysqlQueryFuture::CallbackExecute() {
	if (m_Callback != nullptr)
		m_Callback(&m_Result);
}
