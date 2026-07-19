# Plan: FileLogger 구현

## 목적
- `FileLogger` 클래스를 구현하여 콘솔 외 파일로 로그를 출력할 수 있도록 함
- `InitializeFileLogger` 전역 함수 추가
- jc_gtest 테스트 코드 작성 및 define toggle 설정

## 작업 목록

1. **LoggerAbstract.h** - `LoggerType` enum에 `File` 추가
2. **FileLogger.h** 생성 - FileLogger 클래스 선언
3. **FileLogger.cpp** 생성 - FileLogger 구현
4. **LoggerDefine.h** - `InitializeFileLogger` 선언 추가
5. **LoggerDefine.cpp** - `InitializeFileLogger` 구현 추가
6. **jc.vcxproj** - FileLogger.h/cpp 등록 (ClInclude / ClCompile)
7. **FileLoggerTest.cpp** 생성 - jc_gtest 테스트 코드
8. **jc_gtest.vcxproj** - 테스트 파일 등록
9. **jc_gtest.vcxproj.filters** - 필터 등록 (Logger)
10. **Core.h** - TEST_FileLoggerTest define 추가 (개별/전체 테스트 둘 다)
11. **Build.ps1 빌드 및 테스트 실행**

## FileLogger 설계

- `LoggerAbstract` 상속, `CRuntime::FileOpen("a")`으로 파일 append 모드 사용
- `m_szBuffer`에 버퍼링 후 `Flush()` 시 `CRuntime::FileWrite`로 파일에 기록
- 생성자: 파일 경로 받아서 열기, 소멸자: flush + close
- Header는 VT 색상코드 없이 평문 사용 (base `CreateHeader` 그대로 사용)
- Log/LogPlain 오버라이드 (virtual call 버그 회피)

## 테스트 설계

- 임시 파일(`_Test_FileLogger.log`) 생성 후 로그 write
- Flush 후 파일 읽어서 내용 검증
- 테스트 종료 후 임시 파일 삭제
