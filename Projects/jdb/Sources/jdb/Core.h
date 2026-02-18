/*
 * 작성자: 윤정도
 * 생성일: 2/15/2026 3:04:30 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Core.h>

#include <mysql.h> // 젤 위에 있어야함. 아래 헤더파일들에서 mysql.h에 정의된 타입을 참조하기 때문

#include <jdb/Namespace.h>
#include <jdb/DLLExport.h>

#include <jdb/MySQL/MysqlDatabase.h>
#include <jdb/MySQL/MysqlConnection.h>

#pragma comment(lib, "libmysql.lib")