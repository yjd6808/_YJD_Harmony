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

	SGString Name;			 // DB 접속에 중요한 정보는 아님
	SGString HostName;		 // "127.0.0.1, localhost
	Int16U ConnectionPort;	 // 3306
	int Use[ServerProcessType::Max]; // 서버 프로세스가 DB를 사용하는지 여부
	int ConnectionPoolSize[ServerProcessType::Max];
	SGString AccountId;		 // root
	SGString AccountPass;	 // 1234
	SGString SchemaName;	 // steins-gate
	int MaxConnection[ServerProcessType::Max];
	int IocpThreadCount[ServerProcessType::Max];
	
};
