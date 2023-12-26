/*
 * 작성자: 윤정도
 * 생성일: 12/26/2023 2:20:50 PM
 * =====================
 * ㅠㅠ 생각해보니 내 네트워크 라이브러리는 ICommand 단위로 송수신만 가능하네
 * 그래서 단순 char배열로도 송수신이 가능하도록 파서 클래스를 정의함.
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Declspec.h>

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

class Session;
class JCORE_NOVTABLE PacketParser
{
public:
	PacketParser(Session* session);
	virtual ~PacketParser() = default;

	enum Type
	{
		Raw,				// OnReceivedRaw	-> char		단위로 파싱
		Command,			// OnReceived		-> ICommand 단위로 파싱
		Custom
	};

	virtual void Received(Int32UL receivedBytes) = 0;
	virtual Type GetType() const = 0;

	static PacketParser* Create(int type, Session* session);
protected:
	Session* m_pSession;
};

class PacketRawParser : public PacketParser
{
public:
	PacketRawParser(Session* session);
	void Received(Int32UL receivedBytes) override;
	Type GetType() const override { return Type::Raw; }
};

class PacketCommandParser : public PacketParser
{
public:
	PacketCommandParser(Session* session);
	void Received(Int32UL receivedBytes) override;
	Type GetType() const override { return Type::Command; }
};

NS_JNET_END