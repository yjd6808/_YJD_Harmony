#include <JStudy/Study.h>
#include <JStudy/Server/SynchronizedEchoServer.h>

namespace JStudy {

void SynchronizedEchoServer::Open() {
	TcpServer::Open();
}

void SynchronizedEchoServer::Close() {
	TcpServer::Close();
}

} // namespace JStudy