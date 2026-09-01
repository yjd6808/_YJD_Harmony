/*
 * 작성자: 윤정도
 * 생성일: 8/23/2026
 * =====================
 * 테스트 전용 초경량 HTTP/1.1 목 서버 (블로킹 소켓, 127.0.0.1 전용)
 *  - 검증 대상이 아니므로 의도적으로 단순하게 유지한다 (비동기 서버 스택 미사용)
 */

#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "jc/Threading/Thread.h"

#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

namespace httptest
{
	struct MockRoute
	{
		int         status_ = 200;
		std::string statusText_ = "OK";
		std::string contentType_ = "application/octet-stream";
		std::string body_;                                // 바이너리 안전
		std::map<std::string, std::string> extraHeaders_;
		int         headerDelayMs_ = 0;                   // 응답 헤더 전 지연 — 타임아웃 테스트용
		int         dripChunkSize_ = 0;                   // >0이면 바디를 잘게 나눠 전송
		int         dripIntervalMs_ = 0;                  // drip 청크 간 지연 — 취소 윈도우 확보용
		std::string redirectTo_;                          // 설정 시 302 + Location 응답
		bool        dropConnection_ = false;              // 헤더도 안 보내고 절단 — 연결 오류 테스트용
	};

	struct RecordedRequest
	{
		std::string method_;
		std::string path_;
		std::map<std::string, std::string> headers_;      // 소문자 키로 정규화
		std::string body_;
	};

	class MockHttpServer
	{
	public:
		~MockHttpServer() { Stop(); }

		bool Start(int _port = 0)                         // 0 = OS가 빈 포트 할당
		{
			WSADATA wsaData{};
			if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;

			listenSock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (listenSock_ == INVALID_SOCKET) return false;

			sockaddr_in addr{};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = ::htonl(INADDR_LOOPBACK);
			addr.sin_port = ::htons((u_short)_port);
			if (::bind(listenSock_, (sockaddr*)&addr, sizeof(addr)) != 0) return false;
			if (::listen(listenSock_, SOMAXCONN) != 0) return false;

			int len = sizeof(addr);
			::getsockname(listenSock_, (sockaddr*)&addr, &len);
			port_ = (int)::ntohs(addr.sin_port);

			running_.store(true);
			acceptThread_ = std::thread([this] { AcceptLoop(); });
			return true;
		}

		void Stop()
		{
			if (!running_.exchange(false)) return;
			::closesocket(listenSock_);                   // accept 블로킹 해제
			listenSock_ = INVALID_SOCKET;
			if (acceptThread_.joinable()) acceptThread_.join();
			{
				std::lock_guard<std::mutex> guard(lock_);
				for (auto& worker : workers_)
					if (worker.joinable()) worker.join();
				workers_.clear();
			}
			::WSACleanup();
		}

		int GetPort() const { return port_; }
		std::string GetUri(const std::string& _path) const
		{
			return "http://127.0.0.1:" + std::to_string(port_) + _path;
		}

		void RegisterRoute(const std::string& _path, const MockRoute& _route)
		{
			std::lock_guard<std::mutex> guard(lock_);
			routes_[_path] = _route;
		}

		void RegisterText(const std::string& _path, int _status, const std::string& _body)
		{
			MockRoute route;
			route.status_ = _status;
			route.contentType_ = "text/plain";
			route.body_ = _body;
			RegisterRoute(_path, route);
		}

		int GetRequestCount() const
		{
			std::lock_guard<std::mutex> guard(lock_);
			return (int)requests_.size();
		}

		RecordedRequest GetRequest(int _index) const
		{
			std::lock_guard<std::mutex> guard(lock_);
			return requests_[(size_t)_index];
		}

	private:
		void AcceptLoop()
		{
			for (;;)
			{
				SOCKET client = ::accept(listenSock_, nullptr, nullptr);
				if (client == INVALID_SOCKET) return;     // Stop()에서 listen 소켓이 닫힘
				std::lock_guard<std::mutex> guard(lock_);
				workers_.emplace_back([this, client] { HandleClient(client); });
			}
		}

		void HandleClient(SOCKET _client)
		{
			RecordedRequest request;
			if (ReadRequest(_client, request))
			{
				{
					std::lock_guard<std::mutex> guard(lock_);
					requests_.push_back(request);
				}
				WriteResponse(_client, request);
			}
			::shutdown(_client, SD_BOTH);
			::closesocket(_client);
		}

		static std::string ToLower(std::string _s)
		{
			for (char& c : _s) c = (char)::tolower((unsigned char)c);
			return _s;
		}

		bool ReadRequest(SOCKET _client, RecordedRequest& _request)
		{
			// 1) 헤더 종료(\r\n\r\n)까지 수신
			std::string raw;
			char buf[4096];
			size_t headerEnd = std::string::npos;
			while ((headerEnd = raw.find("\r\n\r\n")) == std::string::npos)
			{
				const int n = ::recv(_client, buf, (int)sizeof(buf), 0);
				if (n <= 0) return false;
				raw.append(buf, (size_t)n);
			}

			// 2) 요청 라인 파싱: "GET /path HTTP/1.1"
			const std::string head = raw.substr(0, headerEnd);
			size_t lineEnd = head.find("\r\n");
			const std::string requestLine = head.substr(0, lineEnd);
			const size_t sp1 = requestLine.find(' ');
			const size_t sp2 = requestLine.find(' ', sp1 + 1);
			if (sp1 == std::string::npos || sp2 == std::string::npos) return false;
			_request.method_ = requestLine.substr(0, sp1);
			_request.path_ = requestLine.substr(sp1 + 1, sp2 - sp1 - 1);

			// 3) 헤더 파싱 (소문자 키)
			size_t cursor = lineEnd + 2;
			while (cursor < head.size())
			{
				size_t next = head.find("\r\n", cursor);
				if (next == std::string::npos) next = head.size();
				const std::string line = head.substr(cursor, next - cursor);
				const size_t colon = line.find(':');
				if (colon != std::string::npos)
				{
					std::string name = ToLower(line.substr(0, colon));
					size_t valueBegin = colon + 1;
					while (valueBegin < line.size() && line[valueBegin] == ' ') ++valueBegin;
					_request.headers_[name] = line.substr(valueBegin);
				}
				cursor = next + 2;
			}

			// 4) Content-Length만큼 바디 수신 (POST 테스트용)
			size_t contentLength = 0;
			const auto it = _request.headers_.find("content-length");
			if (it != _request.headers_.end())
				contentLength = (size_t)std::stoll(it->second);

			_request.body_ = raw.substr(headerEnd + 4);
			while (_request.body_.size() < contentLength)
			{
				const int n = ::recv(_client, buf, (int)sizeof(buf), 0);
				if (n <= 0) return false;
				_request.body_.append(buf, (size_t)n);
			}
			return true;
		}

		void WriteResponse(SOCKET _client, const RecordedRequest& _request)
		{
			MockRoute route;
			{
				std::lock_guard<std::mutex> guard(lock_);
				const auto it = routes_.find(_request.path_);
				if (it != routes_.end()) route = it->second;
				else { route.status_ = 404; route.statusText_ = "Not Found"; route.contentType_ = "text/plain"; route.body_ = "not found"; }
			}

			if (route.dropConnection_) return;            // 그대로 절단 → 클라이언트는 수신/연결 오류
			if (route.headerDelayMs_ > 0) jc::Thread::Sleep(route.headerDelayMs_);

			std::string head;
			if (!route.redirectTo_.empty())
			{
				head = "HTTP/1.1 302 Found\r\n"
				       "Location: " + route.redirectTo_ + "\r\n"
				       "Content-Length: 0\r\n"
				       "Connection: close\r\n\r\n";
				SendAll(_client, head.data(), (int)head.size());
				return;
			}

			head = "HTTP/1.1 " + std::to_string(route.status_) + " " + route.statusText_ + "\r\n"
			       "Content-Type: " + route.contentType_ + "\r\n"
			       "Content-Length: " + std::to_string(route.body_.size()) + "\r\n";
			for (const auto& header : route.extraHeaders_)
				head += header.first + ": " + header.second + "\r\n";
			head += "Connection: close\r\n\r\n";
			SendAll(_client, head.data(), (int)head.size());

			if (route.dripChunkSize_ <= 0)
			{
				SendAll(_client, route.body_.data(), (int)route.body_.size());
				return;
			}

			// drip 전송 — 수신 루프가 여러 번 돌도록 강제 → 진행 통지/취소 윈도우 확보
			int sent = 0;
			while (sent < (int)route.body_.size())
			{
				if (!running_.load()) return;             // 서버 종료 시 중단
				const int n = (std::min)(route.dripChunkSize_, (int)route.body_.size() - sent);
				if (!SendAll(_client, route.body_.data() + sent, n)) return;   // 클라이언트가 끊으면(취소) 종료
				sent += n;
				jc::Thread::Sleep(route.dripIntervalMs_);
			}
		}

		static bool SendAll(SOCKET _socket, const char* _p, int _len)
		{
			int sent = 0;
			while (sent < _len)
			{
				const int n = ::send(_socket, _p + sent, _len - sent, 0);
				if (n <= 0) return false;
				sent += n;
			}
			return true;
		}

		SOCKET listenSock_ = INVALID_SOCKET;
		int port_ = 0;
		std::atomic<bool> running_{ false };
		std::thread acceptThread_;
		std::vector<std::thread> workers_;
		mutable std::mutex lock_;
		std::map<std::string, MockRoute> routes_;
		std::vector<RecordedRequest> requests_;
	};
}
