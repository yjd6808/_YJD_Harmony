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
		DebugAssert(false, "핸들이 제대로 생성되지 않았습니다.");
}

MysqlQueryFuture::~MysqlQueryFuture() {
	if (m_WaitHandle != INVALID_HANDLE_VALUE)
		CloseHandle(m_WaitHandle);

	// 여기서 해제 안해주면 MysqlQuery이녀석 크기만큼 동적할당 해제가 안된다. 그래서 수동으로 호출해줘야함 / 22/04/07 저녁 8시
	Memory::PlacementDeallocate(m_Result);
}

MysqlQuery& MysqlQueryFuture::Wait() {
	DebugAssert(m_WaitHandle != INVALID_HANDLE_VALUE, "핸들이 nullptr 입니다.");
	if (m_bReady == false) {
		// IOCPOverlappedQuery에서 아직 쿼리 결과가 준비가 안된 경우 시그널을 보낼때까지 기다린다.
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
