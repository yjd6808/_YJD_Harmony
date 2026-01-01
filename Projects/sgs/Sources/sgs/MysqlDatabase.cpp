#include "Core.h"
#include "MysqlDatabase.h"

#include <sgs/MysqlConnectionPool.h>
#include <sgs/MysqlStatementBuilder.h>

USING_NS_JC;
USING_NS_JC;
USING_NS_JNET;
USING_NS_STD;

//////////////////////////////////////////////////////////////////////////////////////////
MysqlDatabase::MysqlDatabase(DatabaseInfo* _pInfo)
: iocp_(nullptr)
, connectionPool_(nullptr)
, info_(_pInfo)
, initialized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MysqlDatabase::~MysqlDatabase()
{
	if (initialized_)
	{
		Finalize();
		JCORE_DELETE_SAFE(iocp_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MysqlDatabase::Initialize(ServerProcessType_t _serverProcessType)
{
	if (initialized_)
	{
		_LogError_("이미 생성된 객체입니다.");
		return false;
	}

	const int use = info_->use_[_serverProcessType];
	const int connectionPoolSize = info_->connPoolSize_[_serverProcessType];
	const int maxConnection = info_->maxConnection_[_serverProcessType];
	const int threadCount = info_->iocpThreadCount_[_serverProcessType];

	if (use != 1)
	{
		_LogError_("해당 데이터베이스는 사용하지 않습니다.");
		return false;
	}

	if (connectionPool_ == nullptr)
	{
		connectionPool_ = dbg_new MysqlConnectionPool(
			info_->hostName_,
			info_->connPort_,
			info_->accountId_,
			info_->accountPass_,
			info_->schemaName_,
			maxConnection);
	}

	// 커넥션 풀 초기화
	if (!connectionPool_->Init(connectionPoolSize))
	{
		JCORE_DELETE_SAFE(connectionPool_);
		_LogError_("DB 커넥션 풀 초기화 실패");
		return false;
	}

	// 빌더 커넥션 초기화
	// String Escape 하나를 위해서 어쩔수없이 초기화함;
	if (!MysqlStatementBuilder::Initialize(info_))
	{
		_LogError_("DB 스테이트먼트 빌더 초기화 실패");
		return false;
	}

	_LogInfo_("데이터베이스 커넥션 풀(크기: %d) 초기화 [%s:%d]",
	          connectionPoolSize,
	          info_->hostName_.Source(),
	          info_->connPort_
	);

	iocp_ = dbg_new IOCP(threadCount);
	iocp_->Run();
	initialized_ = true;
	_LogInfo_("%s %s 실행완료 (쓰레드 수: %d)", info_->name_.Source(), IOCP::TypeName(), threadCount);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MysqlDatabase::Finalize()
{
	if (initialized_ == false)
		return;

	_LogInfo_("%s 파괴시작", info_->name_.Source());
	initialized_ = false;

	iocp_->Join();
	_LogInfo_("%s %s 쪼인완료", info_->name_.Source(), IOCP::TypeName());

	iocp_->Destroy();
	_LogInfo_("%s %s 파괴완료", info_->name_.Source(), IOCP::TypeName());

	if (connectionPool_)
		JCORE_DELETE_SAFE(connectionPool_);
	_LogInfo_("%s %s 파괴완료", info_->name_.Source(), IOCP::TypeName(), MysqlConnectionPool::TypeName());

	MysqlStatementBuilder::Finalize();
}
