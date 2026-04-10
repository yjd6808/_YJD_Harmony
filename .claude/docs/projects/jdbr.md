# jdbr (JDatabase Research)

## 개요
**jdb 라이브러리의 테스트 및 데모 실행파일**.  
SQL Server 연결, 쿼리 실행, 비동기 처리, 멀티스레딩을 검증하는 연구 프로젝트.

## 소스 구조

```
Sources/jdbr/
├── Core.h/cpp    - jdb 사용을 위한 문 템플릿 정의 (StmtList)
└── main.cpp      - SQL Server 연결 및 쿼리 테스트 코드
```

## 주요 테스트 내용
- SQL Server 연결/해제
- INSERT / DELETE / SELECT 쿼리 실행
- 비동기 쿼리 + 폴링 모드
- 멀티스레드 동시 쿼리

## 의존성
- `jdb` (데이터베이스 라이브러리)
- `jc`, `jnet` (jdb 전이 의존성)

## 빌드 산출물
- 실행파일 (EXE)
