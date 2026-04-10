# jdb (JDatabase)

## 개요
**MySQL 및 SQL Server 통합 데이터베이스 추상화 라이브러리**.  
동일 인터페이스로 두 DBMS를 지원하며, IOCP 기반 비동기 쿼리 처리 가능.

## 소스 구조

```
Sources/jdb/
├── IConnection.h             - DB 연결 추상 인터페이스
├── IQuery.h                  - 쿼리 실행 추상 인터페이스
├── IDatabase.h               - DB 관리 추상 인터페이스
├── ConnectionPool.h/cpp      - 커넥션 풀 (스레드 안전)
├── QueryDefine.h             - 통합 쿼리 정의 및 매크로
├── QueryHelper.h             - 쿼리 헬퍼 템플릿
├── Structure.h               - DatabaseInfo, BoundStmt 구조체
├── StatementType.h           - SQL 문장 타입 열거형
├── Util.h                    - 플레이스홀더 파싱/바인딩 유틸 (constexpr)
├── MySQL/
│   ├── MysqlConnection.h     - MySQL 연결 (MYSQL* 핸들)
│   ├── MysqlDatabase.h       - MySQL IDatabase 구현
│   ├── MysqlQuery.h          - MySQL 쿼리 4종 (Select/Insert/Update/Delete)
│   └── MysqlStatementBuilder.h
└── SQLServer/
    ├── SqlServerConnection.h - SQL Server 연결 (ODBC)
    ├── SqlServerDatabase.h   - SQL Server IDatabase 구현
    ├── SqlServerQuery.h      - SQL Server 쿼리 4종
    └── SqlServerStatementBuilder.h
```

## 플레이스홀더 문법
```cpp
// {0}, {1}, ... 위치 기반
db.Query(QID, MS_STMT("SELECT * FROM t WHERE id={0} AND name={1}"), id, name);
```

## 지원 타입
`_s8~_u64`, `_f32`, `_f64`, `jc::String`, `const char*`, `jc::DateTime/Date/Time`, `nullptr_t`

## 결과 처리
```cpp
while (query.HasNext()) {
    query.Next();
    _s32 id  = query.GetS32(0);
    String s = query.GetString(1);
}
_s32 insertId  = query.GetInsertId();
_s32 rowCount  = query.GetRowCount();
```

## 비동기 쿼리
- IOCP 기반 (`IQueryTask`)
- 폴링 모드: `db.PollEvents()`
- 완료 콜백: `db.SetQueryCompletedCallback(...)`

## 트랜잭션 (SQL Server 전용)
```cpp
db.BeginTransaction();
// ...
db.CommitTransaction();   // or RollbackTransaction()
```

## 의존성
- `jc` (String, SharedPtr, Container, Logger, Sync 등)
- `jnet` (IOCP 비동기 처리)
- MySQL C API (`libmysql.lib`)
- ODBC (`odbc32.lib`, `msodbcsql.h`)

## 빌드 산출물
- `jdb.lib` (Static Library)
