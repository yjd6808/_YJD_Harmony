#include "gtest/gtest.h"
#include "Core.h"
#include "jc/Io/IoDaemon.h"
#include "jc/Io/Http/HttpRequest.h"
#include "jc/Io/Http/HttpResponse.h"
#include "MockHttpServer.h"
#include "jc/FileSystem/File.h"
#include "jc/Threading/Thread.h"
#include <string>
using namespace jc;
namespace { struct HttpEnv { IoDaemon daemon; httptest::MockHttpServer server; HttpEnv() { IoDaemonConfig cfg; cfg.http_.receiveTimeoutMs_ = 2000; daemon.Initialize(cfg); daemon.Resolver().Mount("save", "test_io/http/save"); server.Start(0); } ~HttpEnv() { server.Stop(); daemon.Shutdown(); } }; }
TEST(HttpServiceTest, SyncDownloadCommits) { HttpEnv env; env.server.RegisterText("/get200", 200, std::string(3000, 120)); auto rr = env.daemon.Resolver().ResolveWritable("save:/get200_body.bin"); File::Delete(rr.fullPath_); IoResultPtr r = env.daemon.Http().Download(env.server.GetUri("/get200"), "save:/get200_body.bin"); ASSERT_NE(r, nullptr); EXPECT_TRUE(r->IsSuccess()); EXPECT_EQ(r->detail_.httpStatus_, 200); EXPECT_TRUE(File::Exist(rr.fullPath_)); }
TEST(HttpServiceTest, SendAsyncKeepsStatusOn404) { HttpEnv env; env.server.RegisterText("/notfound", 404, "nope"); Atomic<bool> done{false}; int seenStatus=0; IoError seenErr=ieNone; env.daemon.Http().SendAsync(HttpRequest(jc::Uri(env.server.GetUri("/notfound").c_str())), [&](const IoResult& rr){ done.Store(true); seenStatus=rr.http_.status_; seenErr=rr.error_; }); int spins=0; while(!done.Load() && spins++<5000){ env.daemon.Update(); jc::Thread::Sleep(1);} EXPECT_TRUE(done.Load()); EXPECT_EQ(seenStatus,404); EXPECT_EQ(seenErr,ieNone); }
TEST(HttpServiceTest, PostBodyAndHeader) { HttpEnv env; env.server.RegisterText("/login", 200, "{}"); std::string json="{\"user\":\"tester\"}"; Atomic<bool> done{false}; HttpRequest req(env.server.GetUri("/login").c_str()); req.SetMethod(HttpMethod::hmPost); req.SetHeader(jc::String("Content-Type"), jc::String("application/json")); req.SetHeader(jc::String("Authorization"), jc::String("Bearer token123")); req.SetBody(jc::String(json.c_str())); env.daemon.Http().SendAsync(req, [&](const IoResult&){ done.Store(true);}); int spins=0; while(!done.Load() && spins++<5000){ env.daemon.Update(); jc::Thread::Sleep(1);} EXPECT_TRUE(done.Load()); EXPECT_GE(env.server.GetRequestCount(),1); auto rec=env.server.GetRequest(0); EXPECT_EQ(rec.method_, "POST"); }
TEST(HttpServiceTest, DownloadNon2xxFails) { HttpEnv env; env.server.RegisterText("/gone", 404, "gone"); Atomic<bool> done{false}; IoError seenErr=ieNone; int seenStatus=0; env.daemon.Http().DownloadAsync(env.server.GetUri("/gone"), "save:/gone.bin", [&](const IoResult& rr){ done.Store(true); seenErr=rr.error_; seenStatus=rr.detail_.httpStatus_;}); int spins=0; while(!done.Load() && spins++<5000){ env.daemon.Update(); jc::Thread::Sleep(1);} EXPECT_TRUE(done.Load()); EXPECT_EQ(seenErr, ieHttpStatusFailed); EXPECT_EQ(seenStatus,404); }
