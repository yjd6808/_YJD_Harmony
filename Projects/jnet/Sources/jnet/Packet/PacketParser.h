/*
 * 작성자: 윤정도
 * 생성일: 12/26/2023 2:20:50 PM
 * =====================
 * ㅠㅠ 생각해보니 내 네트워크 라이브러리는 ICommand 단위로 송수신만 가능하네
 * 그래서 단순 char배열로도 송수신이 가능하도록 파서 클래스를 정의함.
 */


#pragma once

#include "jc/Type.h"
#include "jc/Declspec.h"

#include "jnet/Namespace.h"

NS_JNET_BEGIN

class Session;
class PacketParser
{
public:
	PacketParser(Session* _pSession);
	~PacketParser() = default;

	void Received(_u32l _receivedBytes);
protected:
	Session* session_;
};

NS_END
