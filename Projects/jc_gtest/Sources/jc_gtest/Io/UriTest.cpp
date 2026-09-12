/*
 * 작성자: 윤정도
 * 생성일: 8/23/2026
 * =====================
 * jc::Uri 파싱 단위 테스트
 */

#include "jc/Primitives/Uri.h"

#if TEST_UriTest == ON

static UriScheme GetSchemeOf(const Uri& _uri) { return _uri.GetScheme(); }   // 암묵 변환 확인용

TEST(UriTest, LocalPaths) {
	LeakCheck;

	// 드라이브 절대 경로 (역슬래시)
	{
		Uri uri(_T("C:\\Temp\\file.dat"));
		EXPECT_TRUE(uri.IsValid());
		EXPECT_TRUE(uri.IsLocal());
		EXPECT_FALSE(uri.IsRemote());
		EXPECT_EQ(uri.GetScheme(), UriScheme::usFile);
		EXPECT_TRUE(uri.GetFileName() == _T("file.dat"));
	}

	// 슬래시 표기 로컬 경로
	{
		Uri uri(_T("C:/Temp/file.dat"));
		EXPECT_TRUE(uri.IsValid());
		EXPECT_EQ(uri.GetScheme(), UriScheme::usFile);
		EXPECT_TRUE(uri.GetFileName() == _T("file.dat"));
	}

	// 상대 경로도 로컬로 취급 (기존 FileStream 테스트처럼 CWD 기준)
	{
		Uri uri(_T("Data/file.dat"));
		EXPECT_TRUE(uri.IsValid());
		EXPECT_EQ(uri.GetScheme(), UriScheme::usFile);
	}

	// UNC 경로
	{
		Uri uri(_T("\\\\fileserver\\share\\a.bin"));
		EXPECT_TRUE(uri.IsValid());
		EXPECT_EQ(uri.GetScheme(), UriScheme::usFile);
		EXPECT_TRUE(uri.GetFileName() == _T("a.bin"));
	}
}

TEST(UriTest, FileScheme) {
	LeakCheck;
	Uri uri(_T("file:///C:/Temp/a.dat"));
	EXPECT_TRUE(uri.IsValid());
	EXPECT_TRUE(uri.IsLocal());
	EXPECT_EQ(uri.GetScheme(), UriScheme::usFile);
	EXPECT_TRUE(uri.GetFileName() == _T("a.dat"));
	// 구분자 정규화 규칙은 구현 정의 — 파일명/유효성만 단언하고 구분자 형태는 과다 제약하지 않는다
}

TEST(UriTest, HttpBasic) {
	LeakCheck;
	Uri uri(_T("http://example.com/files/a.bin"));
	EXPECT_TRUE(uri.IsValid());
	EXPECT_TRUE(uri.IsRemote());
	EXPECT_FALSE(uri.IsLocal());
	EXPECT_EQ(uri.GetScheme(), UriScheme::usHttp);
	EXPECT_TRUE(uri.GetHost() == _T("example.com"));
	EXPECT_EQ(uri.GetPort(), 80);                        // 기본 포트
	EXPECT_TRUE(uri.GetPath() == _T("/files/a.bin"));
	EXPECT_TRUE(uri.GetFileName() == _T("a.bin"));
	EXPECT_TRUE(uri.GetQuery().IsEmpty());
}

TEST(UriTest, HttpsWithPortAndQuery) {
	LeakCheck;
	Uri uri(_T("https://example.com:8443/a/b.bin?x=1&y=2"));
	EXPECT_TRUE(uri.IsValid());
	EXPECT_EQ(uri.GetScheme(), UriScheme::usHttps);
	EXPECT_TRUE(uri.GetHost() == _T("example.com"));
	EXPECT_EQ(uri.GetPort(), 8443);                      // 명시 포트 우선
	EXPECT_TRUE(uri.GetPath() == _T("/a/b.bin"));
	EXPECT_TRUE(uri.GetQuery() == _T("x=1&y=2"));
	EXPECT_TRUE(uri.GetPathAndQuery() == _T("/a/b.bin?x=1&y=2"));   // WinHTTP OpenRequest에 그대로 전달되는 값
	EXPECT_TRUE(uri.GetFileName() == _T("b.bin"));           // 쿼리 제외
}

TEST(UriTest, SchemeCaseInsensitive) {
	LeakCheck;
	Uri uri(_T("HTTP://EXAMPLE.COM/a"));
	EXPECT_TRUE(uri.IsValid());
	EXPECT_EQ(uri.GetScheme(), UriScheme::usHttp);
}

TEST(UriTest, NoPathDefaultsToRoot) {
	LeakCheck;
	Uri uri(_T("http://example.com"));
	EXPECT_TRUE(uri.IsValid());
	EXPECT_TRUE(uri.GetPath() == _T("/"));
	EXPECT_TRUE(uri.GetFileName().IsEmpty());
}

TEST(UriTest, InvalidInputs) {
	LeakCheck;

	{
		Uri uri(_T(""));
		EXPECT_FALSE(uri.IsValid());                     // 빈 문자열 — LoadAsync에서 eInvalidUri로 즉시 실패할 입구
	}

	{
		Uri uri(_T("ftp://example.com/a"));                  // _T("://")가 있으나 미지원 스킴 → 로컬 경로로 오판하면 안 됨
		EXPECT_FALSE(uri.IsValid());
		EXPECT_EQ(uri.GetScheme(), UriScheme::usUnknown);
	}
}

TEST(UriTest, ImplicitConversion) {
	LeakCheck;
	// const char* / String → Uri 암묵 변환 — Load("C:\\a.bin") 형태 호출의 근거
	EXPECT_EQ(GetSchemeOf(Uri(_T("http://h/a"))), UriScheme::usHttp);
	EXPECT_EQ(GetSchemeOf(Uri(_T("C:\\a.bin"))), UriScheme::usFile);

	String s(_T("https://h/b"));
	EXPECT_EQ(GetSchemeOf(s), UriScheme::usHttps);
}

#endif // TEST_UriTest == ON
