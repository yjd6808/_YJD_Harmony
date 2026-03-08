-- < SteinsGate 프로젝트 SQL 스크립트 >
-- 작성자 : 윤정도
-- SQL Server 버전

USE master;
GO

IF EXISTS (SELECT name FROM sys.databases WHERE name = N'steinsgate')
    DROP DATABASE steinsgate;
GO

CREATE DATABASE steinsgate
    COLLATE Korean_Wansung_CI_AS;
GO

USE steinsgate;
GO

-- 계정 테이블
IF OBJECT_ID('dbo.t_account', 'U') IS NOT NULL
    DROP TABLE dbo.t_account;
GO

CREATE TABLE dbo.t_account (
    c_uid                   INT             NOT NULL    IDENTITY(1,1),
    c_account_id            VARCHAR(20)     NULL,
    c_account_pass          VARCHAR(20)     NOT NULL,
    c_created               DATETIME2       NOT NULL    DEFAULT SYSDATETIME(),      -- 계정 생성날짜
    c_logined               DATETIME2       NULL        DEFAULT NULL,               -- 계정 접속날짜
    c_last_login_server     TINYINT         NOT NULL    DEFAULT 1,                  -- 마지막 접속한 게임서버 ID

    CONSTRAINT t_account_pk             PRIMARY KEY (c_uid),
    CONSTRAINT t_account_uq_account_id  UNIQUE      (c_account_id)
);
GO


-- 캐릭터 테이블
-- 계정:캐릭터=One:Many
IF OBJECT_ID('dbo.t_character', 'U') IS NOT NULL
    DROP TABLE dbo.t_character;
GO

CREATE TABLE dbo.t_character (
    c_uid           INT             NOT NULL    IDENTITY(1,1),
    c_account_uid   INT             NOT NULL,
    c_name          VARCHAR(20)     NULL,                                           -- 캐릭터 닉넴
    c_type          SMALLINT        NOT NULL,                                       -- 캐릭터 타입
    c_town          SMALLINT        NOT NULL,                                       -- 마을
    c_created       DATETIME2       NOT NULL    DEFAULT SYSDATETIME(),              -- 캐릭터 생성날짜
    c_logined       DATETIME2       NULL        DEFAULT NULL,                       -- 캐릭터 접속날짜
    c_level         SMALLINT        NOT NULL    DEFAULT 1,                          -- 캐릭터 레벨
    c_str           INT             NOT NULL    DEFAULT 0,                          -- 캐릭터 힘
    c_dex           INT             NOT NULL    DEFAULT 0,                          -- 캐릭터 민첩성
    c_vit           INT             NOT NULL    DEFAULT 0,                          -- 캐릭터 생명력
    c_int           INT             NOT NULL    DEFAULT 0,                          -- 캐릭터 지능
    c_hp            INT             NOT NULL    DEFAULT 0,                          -- 캐릭터 HP
    c_mp            INT             NOT NULL    DEFAULT 0,                          -- 캐릭터 MP
    c_left_stat     INT             NOT NULL    DEFAULT 0,                          -- 부여가능한 남은 스탯

    CONSTRAINT t_character_pk           PRIMARY KEY (c_uid),
    CONSTRAINT t_character_uq_name      UNIQUE      (c_name),
    CONSTRAINT t_character_fk_account_id
        FOREIGN KEY (c_account_uid) REFERENCES dbo.t_account(c_uid) ON DELETE CASCADE
);
GO


-- 아이템 어트리뷰트
-- 장비아이템에 부여될 옵션과 수치
-- 아이템:속성=One:One
IF OBJECT_ID('dbo.t_attribute', 'U') IS NOT NULL
    DROP TABLE dbo.t_attribute;
GO

CREATE TABLE dbo.t_attribute (
    c_uid           INT         NOT NULL    IDENTITY(1,1),
    c_base_opt_1    TINYINT     NOT NULL    DEFAULT 0,      -- 기본 옵션 1
    c_base_value_1  SMALLINT    NOT NULL    DEFAULT 0,      -- 기본 옵션 1 수치
    c_base_opt_2    TINYINT     NOT NULL    DEFAULT 0,      -- 기본 옵션 2
    c_base_value_2  SMALLINT    NOT NULL    DEFAULT 0,      -- 기본 옵션 2 수치
    c_opt_1         TINYINT     NOT NULL    DEFAULT 0,      -- 부가 옵션 1
    c_value_1       SMALLINT    NOT NULL    DEFAULT 0,      -- 부가 옵션 1 수치
    c_opt_2         TINYINT     NOT NULL    DEFAULT 0,      -- 부가 옵션 2
    c_value_2       SMALLINT    NOT NULL    DEFAULT 0,      -- 부가 옵션 2 수치
    c_opt_3         TINYINT     NOT NULL    DEFAULT 0,      -- 부가 옵션 3
    c_value_3       SMALLINT    NOT NULL    DEFAULT 0,      -- 부가 옵션 3 수치
    c_opt_4         TINYINT     NOT NULL    DEFAULT 0,      -- 부가 옵션 4
    c_value_4       SMALLINT    NOT NULL    DEFAULT 0,      -- 부가 옵션 4 수치
    c_opt_5         TINYINT     NOT NULL    DEFAULT 0,      -- 부가 옵션 5
    c_value_5       SMALLINT    NOT NULL    DEFAULT 0,      -- 부가 옵션 5 수치
    c_opt_6         TINYINT     NOT NULL    DEFAULT 0,      -- 부가 옵션 6
    c_value_6       SMALLINT    NOT NULL    DEFAULT 0,      -- 부가 옵션 6 수치

    CONSTRAINT t_attribute_pk PRIMARY KEY (c_uid)
);
GO


-- 아이템 테이블
-- 캐릭터:아이템=One:Many
IF OBJECT_ID('dbo.t_item', 'U') IS NOT NULL
    DROP TABLE dbo.t_item;
GO

CREATE TABLE dbo.t_item (
    c_uid           INT         NOT NULL    IDENTITY(1,1),
    c_character_id  INT         NOT NULL,                   -- 아이템 소유 캐릭터 FK
    c_attribute_id  INT         NULL        DEFAULT NULL,   -- 아이템 능력치 FK (장비류)
    c_item_code     INT         NOT NULL,                   -- 아이템 코드
    c_slot          SMALLINT    NOT NULL,                   -- 아이템 슬롯 인덱스
    c_stored        TINYINT     NOT NULL,                   -- 아이템 저장인벤 (장비, 소모품, 기타, 퀘스트, 아바타, 창고)
    c_count         SMALLINT    NOT NULL,                   -- 아이템 갯수

    CONSTRAINT t_item_pk                PRIMARY KEY (c_uid),
    CONSTRAINT t_item_fk_character_id
        FOREIGN KEY (c_character_id) REFERENCES dbo.t_character(c_uid) ON DELETE CASCADE,
    CONSTRAINT t_item_fk_attribute_id
        FOREIGN KEY (c_attribute_id) REFERENCES dbo.t_attribute(c_uid) ON DELETE CASCADE
);
GO


-- [초대 테이블]
-- SGEnum.h의 InvitationType
IF OBJECT_ID('dbo.t_invitation', 'U') IS NOT NULL
    DROP TABLE dbo.t_invitation;
GO

CREATE TABLE dbo.t_invitation (
    c_uid                   INT         NOT NULL    IDENTITY(1,1),
    c_type                  TINYINT     NOT NULL,                               -- 초대 종류
    c_requested             DATETIME2   NOT NULL    DEFAULT SYSDATETIME(),      -- 초대한 날짜
    c_host_character_uid    INT         NOT NULL,                               -- 초대요청자 FK
    c_guest_character_uid   INT         NOT NULL,                               -- 초대수신자 FK

    CONSTRAINT t_invitation_pk                      PRIMARY KEY (c_uid),
    CONSTRAINT t_invitation_fk_host_character_id
        FOREIGN KEY (c_host_character_uid) REFERENCES dbo.t_character(c_uid) ON DELETE CASCADE
);
GO


-- [친구 관계 테이블]
-- 수락, 요청자 합쳐서 PK
IF OBJECT_ID('dbo.t_friendship', 'U') IS NOT NULL
    DROP TABLE dbo.t_friendship;
GO

CREATE TABLE dbo.t_friendship (
    c_req_character_uid     INT         NOT NULL,                               -- 초대요청자 FK
    c_ack_character_uid     INT         NOT NULL,                               -- 초대수락자 FK
    c_matched               DATETIME2   NOT NULL    DEFAULT SYSDATETIME(),      -- 친구맺은날

    CONSTRAINT t_friendship_pk              PRIMARY KEY (c_req_character_uid, c_ack_character_uid),
    CONSTRAINT t_friendship_fk_req_character_id
        FOREIGN KEY (c_req_character_uid) REFERENCES dbo.t_character(c_uid) ON DELETE NO ACTION,
    CONSTRAINT t_friendship_fk_ack_character_id
        FOREIGN KEY (c_ack_character_uid) REFERENCES dbo.t_character(c_uid) ON DELETE NO ACTION
);
GO