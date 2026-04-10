---
paths:
  - "Scripts/**/*.sql"
---

# SQL 코딩 컨벤션

## 키워드 케이스

SQL 예약어는 **대문자** 사용:

```sql
CREATE TABLE dbo.t_account (
    c_uid INT NOT NULL IDENTITY(1,1),
    c_account_id VARCHAR(20) NULL
);
```

## 네이밍 규칙

| 대상 | 규칙 |
|------|------|
| 테이블 | `t_` 접두사 + snake_case (예: `t_account`, `t_character`) |
| 컬럼 | `c_` 접두사 + snake_case (예: `c_uid`, `c_account_id`) |
| PK 제약 | `t_[테이블명]_pk` (예: `t_account_pk`) |
| UNIQUE 제약 | `t_[테이블명]_uq_[컬럼명]` (예: `t_account_uq_account_id`) |
| FK 제약 | `t_[테이블명]_fk_[컬럼명]` (예: `t_character_fk_account_id`) |

## 들여쓰기 및 정렬

- 4 스페이스 들여쓰기
- 컬럼 정의는 데이터 타입과 constraint를 열 맞춤 정렬

```sql
CREATE TABLE dbo.t_account (
    c_uid               INT             NOT NULL    IDENTITY(1,1),
    c_account_id        VARCHAR(20)     NULL,
    c_account_pass      VARCHAR(20)     NOT NULL,
    c_created           DATETIME2       NOT NULL    DEFAULT SYSDATETIME(),
    c_logined           DATETIME2       NOT NULL    DEFAULT SYSDATETIME(),

    CONSTRAINT t_account_pk             PRIMARY KEY (c_uid),
    CONSTRAINT t_account_uq_account_id  UNIQUE      (c_account_id)
);
```

## SQL Server 스타일

**스키마 명시**: 항상 `dbo.` 접두사 사용:
```sql
CREATE TABLE dbo.t_character ( ... );
FOREIGN KEY (...) REFERENCES dbo.t_account(c_uid)
```

**GO 구문**: 각 DDL 문 이후 사용:
```sql
USE steinsgate;
GO

CREATE TABLE dbo.t_account ( ... );
GO

CREATE TABLE dbo.t_character ( ... );
GO
```

**NULL 명시**: 컬럼의 NULL 허용 여부 항상 명시:
```sql
c_account_id    VARCHAR(20)     NULL,
c_account_pass  VARCHAR(20)     NOT NULL,
```

**외래키 삭제 정책**: 명시적으로 지정:
```sql
CONSTRAINT t_character_fk_account_id
    FOREIGN KEY (c_account_uid) REFERENCES dbo.t_account(c_uid) ON DELETE CASCADE

-- 순환 참조는 NO ACTION 사용
CONSTRAINT t_friendship_fk_target
    FOREIGN KEY (c_target_uid) REFERENCES dbo.t_character(c_uid) ON DELETE NO ACTION
```

## MySQL 스타일

**키워드**: 대문자 사용 (공통 규칙 동일)

**AUTO_INCREMENT 및 문자셋**:
```sql
CREATE SCHEMA steinsgate CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

CREATE TABLE t_account (
    c_uid       INT         PRIMARY KEY AUTO_INCREMENT  COMMENT '계정 PK',
    c_account_id VARCHAR(20) UNIQUE                     COMMENT '계정 아이디'
);
```

**COMMENT**: 컬럼 설명은 `COMMENT` 키워드로 추가:
```sql
c_uid   INT     PRIMARY KEY AUTO_INCREMENT  COMMENT '계정 PK',
```

## 주석 스타일

`--` 스타일 사용 (표준):
```sql
-- 계정 테이블
CREATE TABLE dbo.t_account ( ... );

-- 계정:캐릭터=One:Many
-- 아이템:속성=One:One
```

복잡한 구조 설명:
```sql
-- 아이템 코드 비트 구조
-- 31     26      22       15       0
-- +-------+-------+-------+--------+
-- |  Type |Detail2|Detail1|  Code  |
-- +-------+-------+-------+--------+
-- 자세한 구조는 SGStruct.h 파일 참고
```

파일 헤더:
```sql
-- < SteinsGate 프로젝트 SQL 스크립트 >
-- 작성자 : 윤정도
```
