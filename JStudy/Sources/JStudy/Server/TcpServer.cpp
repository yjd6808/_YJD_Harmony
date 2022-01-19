#include <JStudy/Study.h>
#include <JStudy/Server/TcpServer.h>


namespace JStudy {

/* =======================================================
	Protected
   ======================================================= */

bool TcpServer::Bind() {
	return false;
}

bool TcpServer::Listen() {
	return false;
}

/* ======================================================= 
	Public
   ======================================================= */

void TcpServer::Open() {
	try {


	} catch (std::exception& ex) {
	}
}

void TcpServer::Close() {

}


} // namespace JStudy

