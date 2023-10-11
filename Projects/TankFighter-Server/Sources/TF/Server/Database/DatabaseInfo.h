/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 3:20:05 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/String.h>

struct DatabaseInfo
{
	JCore::String Name;				// DB 접속에 중요한 정보는 아님
	JCore::String HostName;			// "127.0.0.1, localhost
	Int16U ConnectionPort;			// 3306
	int Use;						// 서버 프로세스가 DB를 사용하는지 여부
	int ConnectionPoolSize;
	JCore::String AccountId;		// root
	JCore::String AccountPass;		// 1234
	JCore::String SchemaName;		// steinsgate
	int MaxConnection;
	int IocpThreadCount;

};

