#pragma once

#include <exception>

/*
	열혈강의 TCP/IP 학습차 만든 코드입니다.

	파일명 : JStudyServerBase.h
	작성자 : 윤정도
	작성일 : 22/01/03(월) 오전 10시

	클래스 설명
	 서버 클래스의 최상의 부모 클래스입니다.
	 이 클래스를 상속받아 TcpServer와 UdpServer 클래스를 구현하도록 합니다.
*/

namespace JStudy {

class ServerBase
{
protected:
	virtual void Initialize();
	virtual void Open() = 0;
	virtual void Close() = 0;
};

} // namespace JStudy

