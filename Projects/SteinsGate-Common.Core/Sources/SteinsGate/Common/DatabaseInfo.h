/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:20:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct DatabaseInfo : ConfigDataAbstract
{
	~DatabaseInfo() override = default;

	SGString name_; // DB 접속에 중요한 정보는 아님
	SGString hostName_; // "127.0.0.1, localhost
	Int16U connPort_; // 3306
	int use_[ServerProcessType::Count]; // 서버 프로세스가 DB를 사용하는지 여부
	int connPoolSize_[ServerProcessType::Count];
	SGString accountId_; // root
	SGString accountPass_; // 1234
	SGString schemaName_; // steinsgate
	int maxConnection_[ServerProcessType::Count];
	int iocpThreadCount_[ServerProcessType::Count];
};
