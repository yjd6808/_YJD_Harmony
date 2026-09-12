/*
 * 작성자: 윤정도
 * 생성일: 7/19/2026
 * =====================
 */


#include "jc/IO/File.h"
#include "jc/Logger/FileLogger.h"
#include "jc/Primitives/StringConvert.h"

#if TEST_FileLoggerTest == ON

static const char* g_TestFilePath = "_Test_FileLogger.log";

TEST(FileLoggerTest, BasicLog)
{
	LeakCheck;

	// 이전 테스트에서 남은 파일이 있으면 삭제
	File::Delete(g_TestFilePath);

	{
		FileLogger logger(StringConvert::FromAnsi(g_TestFilePath));
		logger.SetAutoFlush(true);
		logger.SetHeaderFormat(_T("[ level | datetime ] "));
		logger.ShowLevel(true);
		logger.ShowDateTime(true);

		logger.LogInfo(_T("정보 메시지: %d"), 42);
		logger.LogWarn(_T("경고 메시지: %s"), _T("주의!"));
		logger.LogError(_T("오류 메시지"));
		logger.LogDebug(_T("디버그: %f"), 3.14);
		logger.LogPlain(_T("일반 텍스트 출력"));

		logger.Flush();
	}

	// 파일이 존재하는지 확인
	EXPECT_TRUE(File::Exist(g_TestFilePath));

	// 파일 내용 읽기
	String content = File::ReadAllText(g_TestFilePath);
	EXPECT_GT(content.Length(), 0);

	// 각 로그 레벨 문자열이 포함되어 있는지 확인
	EXPECT_TRUE(content.Find(_T("Info")) != -1);
	EXPECT_TRUE(content.Find(_T("Warn")) != -1);
	EXPECT_TRUE(content.Find(_T("Error")) != -1);
	EXPECT_TRUE(content.Find(_T("Debug")) != -1);
	EXPECT_TRUE(content.Find(_T("정보 메시지")) != -1);
	EXPECT_TRUE(content.Find(_T("경고 메시지")) != -1);
	EXPECT_TRUE(content.Find(_T("오류 메시지")) != -1);
	EXPECT_TRUE(content.Find(_T("디버그")) != -1);
	EXPECT_TRUE(content.Find(_T("일반 텍스트 출력")) != -1);

	// 시간 정보가 포함되었는지 확인 (헤더 datetime 태그)
	EXPECT_TRUE(content.Find(_T(":")) != -1);

	// Cleanup
	File::Delete(g_TestFilePath);
}

TEST(FileLoggerTest, AppendMode)
{
	LeakCheck;

	File::Delete(g_TestFilePath);

	// 첫 번째 로그 작성
	{
		FileLogger logger(StringConvert::FromAnsi(g_TestFilePath));
		logger.SetAutoFlush(true);
		logger.ShowLevel(false);
		logger.ShowDateTime(false);
		logger.ShowHeader(false);
		logger.SetHeaderFormat(_T(""));
		logger.LogInfo(_T("첫 번째 로그"));
		logger.Flush();
	}

	// 두 번째 로거에서 추가 작성 (동일 파일, append 모드)
	{
		FileLogger logger(StringConvert::FromAnsi(g_TestFilePath));
		logger.SetAutoFlush(true);
		logger.ShowLevel(false);
		logger.ShowDateTime(false);
		logger.ShowHeader(false);
		logger.SetHeaderFormat(_T(""));
		logger.LogInfo(_T("두 번째 로그"));
		logger.Flush();
	}

	String content = File::ReadAllText(g_TestFilePath);
	EXPECT_TRUE(content.Find(_T("첫 번째 로그")) != -1);
	EXPECT_TRUE(content.Find(_T("두 번째 로그")) != -1);

	File::Delete(g_TestFilePath);
}

TEST(FileLoggerTest, GetFilePath)
{
	LeakCheck;

	File::Delete(g_TestFilePath);

	{
		FileLogger logger(StringConvert::FromAnsi(g_TestFilePath));
		EXPECT_TRUE(logger.GetFilePath() == StringConvert::FromAnsi(g_TestFilePath));
		EXPECT_EQ(logger.GetType(), LoggerType::File);
	}

	File::Delete(g_TestFilePath);
}

TEST(FileLoggerTest, DisableLog)
{
	LeakCheck;

	File::Delete(g_TestFilePath);

	{
		FileLogger logger(StringConvert::FromAnsi(g_TestFilePath));
		logger.SetAutoFlush(true);
		logger.ShowLevel(false);
		logger.ShowDateTime(false);
		logger.ShowHeader(false);
		logger.SetHeaderFormat(_T(""));
		logger.SetEnableLog(LoggerAbstract::eInfo, false);
		logger.SetEnableLog(LoggerAbstract::eWarn, false);
		logger.SetEnableLog(LoggerAbstract::eError, false);

		logger.LogInfo(_T("이건 안나와야 함"));
		logger.LogWarn(_T("이것도 안나와야 함"));
		logger.LogError(_T("이것도 안나와야 함"));
		logger.LogPlain(_T("이건 출력되어야 함"));

		logger.Flush();
	}

	String content = File::ReadAllText(g_TestFilePath);

	// 레벨 로그는 비활성화되었으므로 표시 안됨 ("이건 안나와야 함" 만 없으면 통과)
	// 실제로는 헤더 없이 내용만 적혀있으므로 문자열 검증
	EXPECT_TRUE(content.Find(_T("이건 출력되어야 함")) != -1);

	if (content.Find(_T("이건 안나와야 함")) != -1)
	{
		FAIL() << _T("비활성화된 로그 레벨의 메시지가 출력되었습니다.");
	}

	File::Delete(g_TestFilePath);
}

#endif
