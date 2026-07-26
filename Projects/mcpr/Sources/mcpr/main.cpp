#include <mcpr/PCH/Core.h>

#include <mcp/Protocol/MCPServer.h>
#include <mcp/Protocol/MCPConstants.h>
#include <mcp/Transport/MCPFrameBuffer.h>
#include <mcp/JsonRpc/JsonRpcMessage.h>

#include <cstdio>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <exception>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

static int g_pass = 0;
static int g_fail = 0;
static const int TEST_PORT = 9191;
static MCPServer* g_server = nullptr;
static std::ofstream g_log;

#define LOG(x)     do { g_log << x << std::endl; } while(0)
#define TEST(x)    do { std::fprintf(stderr, "  %-43s ... ", x); LOG("TEST: " << x); } while(0)
#define PASS       do { std::fprintf(stderr, "PASS\n"); ++g_pass; LOG("PASS"); } while(0)
#define FAIL(x)    do { std::fprintf(stderr, "FAIL: %s\n", x); LOG("FAIL: " << x); ++g_fail; return; } while(0)
#define CHECK(c,x) do { if (!(c)) { FAIL(x); return; } } while(0)
#define CHECK_EQ(a,b,x) do { if ((a) != (b)) { FAIL(x); return; } } while(0)

struct TestClient
{
    SOCKET sock = INVALID_SOCKET;

    bool Connect(int _port)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) return false;
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_port);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
        return connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0;
    }

    void Send(const std::string& _json)
    {
        auto frame = MCPFrameBuffer::MakeFrame(_json);
        send(sock, frame.data(), (int)frame.size(), 0);
    }

    std::string Recv(int _timeoutMs = 5000)
    {
        DWORD tv = _timeoutMs;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

        char header[4];
        int n = recv(sock, header, 4, MSG_WAITALL);
        if (n != 4) { return ""; }

        int len = ((unsigned char)header[0] << 24) |
                  ((unsigned char)header[1] << 16) |
                  ((unsigned char)header[2] << 8)  |
                  (unsigned char)header[3];
        if (len <= 0 || len > 1024 * 1024) return "";

        std::string payload(len, '\0');
        n = recv(sock, &payload[0], len, MSG_WAITALL);
        if (n != len) return "";
        return payload;
    }

    void Close()
    {
        if (sock != INVALID_SOCKET)
            closesocket(sock);
        sock = INVALID_SOCKET;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
static Json::Value ParseJson(const std::string& _raw)
{
    Json::Value root;
    if (_raw.empty()) return root;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream ss(_raw);
    Json::parseFromStream(reader, ss, &root, &errs);
    return root;
}

//////////////////////////////////////////////////////////////////////////////////////////
static void RunTests()
{
    TestClient client;
    CHECK(client.Connect(TEST_PORT), "connect to server");
    std::fprintf(stderr, "\n");

    // Test 1: Initialize
    {
        TEST("initialize request/response");
        Json::Value params;
        params["protocolVersion"] = "2024-11-05";
        params["capabilities"] = Json::Value();
        params["clientInfo"]["name"] = "mcpr-test";
        params["clientInfo"]["version"] = "1.0.0";
        client.Send(JsonRpcMessage::MakeRequest("initialize", params, "1").ToJson());
        for (int i = 0; i < 10; i++) { g_server->PollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
        Json::Value res = ParseJson(client.Recv(2000));
        CHECK(!res.isNull(), "response is not empty");
        CHECK_EQ(res["id"].asString(), "1", "response id matches");
        CHECK_EQ(res["jsonrpc"].asString(), "2.0", "jsonrpc version");
        CHECK(res.isMember("result"), "has result field");
        CHECK_EQ(res["result"]["protocolVersion"].asString(), MCPProtocol::kVersion, "protocol version matches");
        CHECK_EQ(res["result"]["serverInfo"]["name"].asString(), MCPProtocol::kProtocolName, "server name matches");
        CHECK_EQ(res["result"]["serverInfo"]["version"].asString(), MCPProtocol::kVersion, "server version matches");
        CHECK(res["result"]["capabilities"]["tools"]["supported"].asBool(), "tools capability supported");
        PASS;
    }

    // Test 2: Tools/List
    {
        TEST("tools/list request/response");
        Json::Value params;
        client.Send(JsonRpcMessage::MakeRequest("tools/list", params, "2").ToJson());
        for (int i = 0; i < 10; i++) { g_server->PollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
        Json::Value res = ParseJson(client.Recv(2000));
        CHECK(!res.isNull(), "response is not empty");
        CHECK_EQ(res["id"].asString(), "2", "response id matches");
        CHECK(res.isMember("result"), "has result field");
        CHECK(res["result"].isMember("tools"), "has tools array");
        CHECK(res["result"]["tools"].isArray(), "tools is array");
        CHECK(res["result"]["tools"].size() >= 2, "at least 2 tools registered");

        bool hasEcho = false, hasPing = false;
        for (auto& t : res["result"]["tools"])
        {
            std::string name = t["name"].asString();
            if (name == "echo") hasEcho = true;
            if (name == "ping") hasPing = true;
        }
        CHECK(hasEcho, "echo tool registered");
        CHECK(hasPing, "ping tool registered");
        PASS;
    }

    // Test 3: Echo tool with custom message
    {
        TEST("tools/call echo with message");
        Json::Value params;
        params["name"] = "echo";
        params["arguments"]["message"] = "Hello MCP!";
        client.Send(JsonRpcMessage::MakeRequest("tools/call", params, "3").ToJson());
        for (int i = 0; i < 10; i++) { g_server->PollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
        Json::Value res = ParseJson(client.Recv(2000));
        CHECK(!res.isNull(), "response is not empty");
        CHECK_EQ(res["id"].asString(), "3", "response id matches");
        CHECK(res.isMember("result"), "has result field");
        CHECK_EQ(res["result"]["message"].asString(), "Hello MCP!", "echo returns same message");
        PASS;
    }

    // Test 4: Ping tool
    {
        TEST("tools/call ping returns pong");
        Json::Value params;
        params["name"] = "ping";
        params["arguments"] = Json::Value();
        client.Send(JsonRpcMessage::MakeRequest("tools/call", params, "4").ToJson());
        for (int i = 0; i < 10; i++) { g_server->PollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
        Json::Value res = ParseJson(client.Recv(2000));
        CHECK(!res.isNull(), "response is not empty");
        CHECK_EQ(res["id"].asString(), "4", "response id matches");
        CHECK(res.isMember("result"), "has result field");
        CHECK(res["result"]["pong"].asBool(), "pong is true");
        PASS;
    }

    // Test 5: Echo default message
    {
        TEST("tools/call echo without message (default)");
        Json::Value params;
        params["name"] = "echo";
        params["arguments"] = Json::Value();
        client.Send(JsonRpcMessage::MakeRequest("tools/call", params, "5").ToJson());
        for (int i = 0; i < 10; i++) { g_server->PollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
        Json::Value res = ParseJson(client.Recv(2000));
        CHECK(!res.isNull(), "response is not empty");
        CHECK_EQ(res["id"].asString(), "5", "response id matches");
        CHECK_EQ(res["result"]["message"].asString(), "hello", "echo returns default message");
        PASS;
    }

    // Test 6: Unknown tool returns error
    {
        TEST("tools/call unknown tool returns error");
        Json::Value params;
        params["name"] = "nonexistent_tool";
        params["arguments"] = Json::Value();
        client.Send(JsonRpcMessage::MakeRequest("tools/call", params, "6").ToJson());
        for (int i = 0; i < 10; i++) { g_server->PollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
        Json::Value res = ParseJson(client.Recv(2000));
        CHECK(!res.isNull(), "response is not empty");
        CHECK_EQ(res["id"].asString(), "6", "response id matches");
        CHECK(res.isMember("error"), "has error field");
        CHECK_EQ(res["error"]["code"].asInt(), -32603, "InternalError for unknown tool");
        PASS;
    }

    client.Close();
}

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
    Winsock::Initialize(2, 2);
    InitializeJCore(_argc, _argv);
    InitializeNetLogger();

    g_log.open("C:\\Users\\jdyun\\AppData\\Local\\Temp\\opencode\\mcpr_int2.log");
    g_log << "=== starting ===" << std::endl;
    std::fprintf(stderr, "=== MCP Server/Client Integration Test ===\n\n");

    try
    {
        jnet::InitializeJNet(0, nullptr);

        std::fprintf(stderr, "[Setup] Starting server...\n");
        g_server = MCPServer::Create();
        bool started = g_server->Start(TEST_PORT);
        if (!started) { std::fprintf(stderr, "FAIL: server start failed\n"); return 1; }
        std::fprintf(stderr, "[Setup] Server started on port %d\n\n", TEST_PORT);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        RunTests();

        g_server->RequestShutdown();
        g_server->Destroy();
        jnet::FinalizeJNet();
    }
    catch (const std::exception& e)
    {
        g_log << "EXCEPTION: " << e.what() << std::endl;
        std::fprintf(stderr, "EXCEPTION: %s\n", e.what());
    }
    catch (...)
    {
        g_log << "UNKNOWN EXCEPTION" << std::endl;
        std::fprintf(stderr, "UNKNOWN EXCEPTION\n");
    }

    std::fprintf(stderr, "\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    g_log << "=== exiting ===" << std::endl;
    g_log.close();
    return g_fail > 0 ? 1 : 0;
}
