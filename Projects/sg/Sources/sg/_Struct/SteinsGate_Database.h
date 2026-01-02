/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:20:01 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>

 //////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(DatabaseType)
	Game = 1
JC_SENUM_END(DatabaseType)

//////////////////////////////////////////////////////////////////////////////////////////
struct DatabaseInfo : SDescBase
{
	~DatabaseInfo() override = default;

	jc::String name_; // DB 접속에 중요한 정보는 아님
	jc::String hostName_; // "127.0.0.1, localhost
	Int16U connPort_; // 3306
	int use_[ServerProcessType::Count + 1]{}; // 서버 프로세스가 DB를 사용하는지 여부 (0번 인덱스는 쓰지 않음.)
	int connPoolSize_[ServerProcessType::Count + 1]{};
	jc::String accountId_; // root
	jc::String accountPass_; // 1234
	jc::String schemaName_; // steinsgate
	int maxConnection_[ServerProcessType::Count + 1]{};
	int iocpThreadCount_[ServerProcessType::Count + 1]{};
};
