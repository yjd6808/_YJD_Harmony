/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/TypeTraits.h>

#include <tuple>

namespace JNetwork {

/*=====================================================================================
									Ŀ�ǵ� ��ü
						 ��Ŷ�� Ŀ�ǵ带 ��Ƽ� �����Ѵ�.
 =====================================================================================*/

struct ICommand
{
	Int16U Cmd;
	Int16U CmdLen;
};

template <typename T>
struct Command : ICommand { 
	Command(const Int16U cmd) {
		Cmd		= cmd;
		CmdLen	= sizeof(T);
	}

	T Value;
};


/*=====================================================================================
								��Ŷ ��ü
					1�� �̻��� Ŀ�ǵ带 ��Ƽ� ������ �� �ֵ����Ѵ�.
					���ø� �Ķ���ͷ� ��� Command�� ��ӹ��� Ÿ���� �����ϵ��� �ؾ��Ѵ�. 
					(�ٸ� Ÿ���� ���� ���ϵ��� ���Ƴ���)

					SendAsync()�� ��Ŷ�� �۽��ϰ� �Ǹ� IOCPOverlappedSend���� �ش� ��Ŷ�� �Ҹ��Ű���� �Ѵ�.
 =====================================================================================*/

struct IPacket
{
	virtual char*	GetBuf() const = 0;
	virtual Int32U	GetPacketLength() const = 0;
	virtual Int32U	GetCommandCount() const = 0;
	virtual WSABUF	GetWSABuf() const = 0;
	virtual void	Release() { delete this; }
};

template <typename... Types>
class PacketHeader : public IPacket
{
public:
	PacketHeader() {
		m_iCommandCount = sizeof...(Types);
		m_iPacketLen = (... + sizeof(Types));
	}
	virtual ~PacketHeader() {}
public:
	virtual Int32U GetPacketLength() const { return m_iPacketLen;		}
	virtual Int32U GetCommandCount() const { return m_iCommandCount;	}
protected:
	Int32U m_iCommandCount;
	Int32U m_iPacketLen;

	static constexpr int PACKET_LEN = (... + sizeof(Types));
};



template <typename... CommandArgs>
class Packet : public PacketHeader<CommandArgs...>
{
	// Ŀ�ǵ�� �ּ� 1�� �̻� �����ϵ��� ����.
	static_assert(sizeof...(CommandArgs) > 0, 
		"... Packet must have one more command");		

	// ���ø� �Ķ���ͷ� ������ ��� Ÿ���� ICommand�� ��ӹ޾ƾ��Ѵ�.
	static_assert(JCore::IsBaseOf_1Base_MultipleDerived_v<ICommand, CommandArgs...>, 
		"... CommandArgs must be derived type of \"ICommand\"");		

	// ���ڷ� ���޹��� Ŀ�ǵ� Ÿ��
	template <int Index>
	using TypeAt		= typename std::tuple_element_t<Index, std::tuple<CommandArgs...>>;	
	using TPacketHeader = typename PacketHeader<CommandArgs...>;
public:
	Packet() : TPacketHeader(), m_pBuf("") {}
	virtual ~Packet() {}
public:
	template <int Index>
	constexpr int SumOfSize() {
		if constexpr (Index == 0) {
			return sizeof(TypeAt<0>);
		} else {
			return sizeof(TypeAt<Index>) + SumOfSize<Index - 1>();
		}
	}

	template <int Index>
	constexpr TypeAt<Index>* Get() {

		static_assert(Index < sizeof...(CommandArgs), 
			"... Index must be less than parameter pack count");

		if constexpr (Index == 0) {
			return (TypeAt<Index>*)m_pBuf;
		} else {
			return (TypeAt<Index>*)(m_pBuf + SumOfSize<Index - 1>());
		}
	}

	virtual char*  GetBuf() const { return (char*)m_pBuf; }
	virtual WSABUF GetWSABuf() const { return { this->GetPacketLength(), GetBuf() }; }

private:
	char m_pBuf[TPacketHeader::PACKET_LEN];
};

} // namespace JNetwork