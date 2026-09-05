#include "gtest/gtest.h"
#include "Core.h"
#include "jc/IO/Engine/IODaemon.h"
#include "jc/IO/Http/HttpRequest.h"
#include "jc/IO/Http/HttpResponse.h"
#include "MockHttpServer.h"
#include "jc/IO/File.h"
#include "jc/Threading/Thread.h"
#include <string>
using namespace jc;
namespace { struct HttpEnv { IODaemon daemon; httptest::MockHttpServer server; HttpEnv() { IODaemonConfig cfg; cfg.http_.receiveTimeoutMs_ = 2000; daemon.Initialize(cfg); daemon.Resolver().Mount("save", "test_io/http/save"); server.Start(0); } ~HttpEnv() { server.Stop(); daemon.Shutdown(); } };

bool VerifyHttpPattern(const _byte* _p, int _len)
{
	for (int i = 0; i < _len; ++i)
	{
		if (_p[i] != (_byte)(i % 251))
			return false;
	}
	return true;
}

std::string MakePatternBody(int _len)
{
	std::string s;
	s.resize(_len);
	for (int i = 0; i < _len; ++i)
		s[i] = (char)(i % 251);
	return s;
}

void PumpUntil(IODaemon& _daemon, Atomic<bool>& _done)
{
	int spins = 0;
	while (!_done.Load() && spins++ < 5000)
	{
		_daemon.Update();
		jc::Thread::Sleep(1);
	}
}
}

TEST(HttpServiceTest, GetWritesCallerResponseBody)
{
	HttpEnv env;
	env.server.RegisterText("/get200", 200, MakePatternBody(3000));

	// 호출자 그릇에 직접 기록 — 결과 회수 단계 없음 (§1)
	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	IOResult r = env.daemon.Http().Get(env.server.GetUri("/get200").c_str(), spRes);

	EXPECT_TRUE(r.IsOk());
	EXPECT_EQ(r.bytesTransferred_, 3000u);
	EXPECT_EQ(spRes->GetStatusCode(), 200);
	ASSERT_NE(spRes->GetBody(), nullptr);
	EXPECT_EQ(spRes->GetBody()->GetSize(), 3000u);
	EXPECT_TRUE(VerifyHttpPattern(spRes->GetBody()->GetData(), 3000));
}

TEST(HttpServiceTest, SendAsyncKeepsStatusOn404)
{
	HttpEnv env;
	env.server.RegisterText("/notfound", 404, "nope");

	// 메모리 전송: 404도 전송 성공 — 상태 코드는 _spResponse로 판단 (R5/R4)
	Atomic<bool> done{ false };
	int seenStatus = 0;
	IOError seenErr = ieNone;
	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	env.daemon.Http().SendAsync(HttpRequest(jc::Uri(env.server.GetUri("/notfound").c_str())), spRes, [&](const IOResult& rr)
	{
		done.Store(true);
		seenStatus = spRes->GetStatusCode();
		seenErr = rr.error_;
	});

	PumpUntil(env.daemon, done);
	EXPECT_TRUE(done.Load());
	EXPECT_EQ(seenStatus, 404);
	EXPECT_EQ(seenErr, ieNone);
}

TEST(HttpServiceTest, PostBodyAndHeader)
{
	HttpEnv env;
	env.server.RegisterText("/login", 200, "{}");
	std::string json = "{\"user\":\"tester\"}";

	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	MemoryStreamPtr spBody = MakeShared<MemoryStream>(0u);
	spBody->Write(json.data(), (_u32)json.size());
	IOResult r = env.daemon.Http().Post(env.server.GetUri("/login").c_str(), spBody, String("application/json"), spRes);

	EXPECT_TRUE(r.IsOk());
	EXPECT_EQ(spRes->GetStatusCode(), 200);

	ASSERT_GE(env.server.GetRequestCount(), 1);
	auto rec = env.server.GetRequest(0);
	EXPECT_EQ(rec.method_, "POST");
}

TEST(HttpServiceTest, PostAsyncBodyAndHeader)
{
	HttpEnv env;
	env.server.RegisterText("/login2", 200, "{}");
	std::string json = "{\"user\":\"tester\"}";

	Atomic<bool> done{ false };
	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	MemoryStreamPtr spBody = MakeShared<MemoryStream>(0u);
	spBody->Write(json.data(), (_u32)json.size());
	env.daemon.Http().PostAsync(env.server.GetUri("/login2").c_str(), spBody, String("application/json"), spRes,
		[&](const IOResult&) { done.Store(true); });

	PumpUntil(env.daemon, done);
	EXPECT_TRUE(done.Load());
	EXPECT_EQ(spRes->GetStatusCode(), 200);

	ASSERT_GE(env.server.GetRequestCount(), 1);
	auto rec = env.server.GetRequest(0);
	EXPECT_EQ(rec.method_, "POST");
}

TEST(HttpServiceTest, DownloadCommitsFileAndPreservesStatus)
{
	HttpEnv env;
	env.server.RegisterText("/get200", 200, MakePatternBody(3000));
	auto rr = env.daemon.Resolver().ResolveWritable("save:/get200_body.bin");
	File::Delete(rr.fullPath_);

	// _spResponse 전달 — 상태/헤더 필요 시 보존
	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	IOResult r = env.daemon.Http().Download(env.server.GetUri("/get200"), "save:/get200_body.bin", spRes);

	EXPECT_TRUE(r.IsOk());
	EXPECT_EQ(r.bytesTransferred_, 3000u);
	EXPECT_EQ(spRes->GetStatusCode(), 200);
	EXPECT_TRUE(File::Exist(rr.fullPath_));
}

TEST(HttpServiceTest, DownloadNon2xxFailsWithStatusPreserved)
{
	HttpEnv env;
	env.server.RegisterText("/gone", 404, "gone");

	// 파일 다운로드는 2xx만 성공 — 상태 코드는 손실 없이 보존 (R4)
	Atomic<bool> done{ false };
	IOError seenErr = ieNone;
	int seenStatus = 0;
	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	env.daemon.Http().DownloadAsync(env.server.GetUri("/gone"), "save:/gone.bin", spRes, [&](const IOResult& rr)
	{
		done.Store(true);
		seenErr = rr.error_;
		seenStatus = spRes->GetStatusCode();
	});

	PumpUntil(env.daemon, done);
	EXPECT_TRUE(done.Load());
	EXPECT_EQ(seenErr, ieHttpStatusFailed);
	EXPECT_EQ(seenStatus, 404);
}

TEST(HttpServiceTest, SendWithRequestObject)
{
	// 저수준 공통 진입점 — REST 표현력 유지
	HttpEnv env;
	env.server.RegisterText("/login", 200, "{}");
	std::string json = "{\"user\":\"tester\"}";

	HttpRequest req(env.server.GetUri("/login").c_str());	// 사용자 예제 패턴 그대로
	req.SetMethod(HttpMethod::hmPost)
	   .SetHeader(jc::String("Content-Type"), jc::String("application/json"))
	   .SetHeader(jc::String("Authorization"), jc::String("Bearer token123"));
	req.SetBody(jc::String(json.c_str()));

	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	IOResult r = env.daemon.Http().Send(req, spRes);

	EXPECT_TRUE(r.IsOk());
	EXPECT_EQ(spRes->GetStatusCode(), 200);

	ASSERT_GE(env.server.GetRequestCount(), 1);
	auto rec = env.server.GetRequest(0);
	EXPECT_EQ(rec.method_, "POST");
	EXPECT_EQ(rec.headers_["content-type"], "application/json");
	EXPECT_EQ(rec.headers_["authorization"], "Bearer token123");
	EXPECT_EQ(rec.body_, json);
}
