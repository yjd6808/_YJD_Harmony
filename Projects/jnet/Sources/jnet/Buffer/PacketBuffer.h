/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:30:33 AM
 * =====================
 */


#pragma once

#include <jnet/Buffer/PooledStreamBuffer.h>
#include <jnet/Packet/Command.h>

NS_JNET_BEGIN

class PacketBuffer : public PooledStreamBuffer
{
public:
	PacketBuffer(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize);
	PacketBuffer(const PacketBuffer& _other);
	virtual ~PacketBuffer() override = default;

	jc::CMessage EmplaceMsg();
	void		 CompleteMsg();

	void		 EmplaceCmd(ICommand* _pCmd);

	template <typename TCommand>
	TCommand& EmplaceCmd()
	{
		static_assert(jc::IsBaseOf_v<StaticCommand, TCommand>, "... this method is only for StaticCommand ");
		return EmplaceCmd<TCommand>(0);
	}

	template <typename TCommand>
	TCommand& EmplaceCmd(int _count)
	{
		CMD_CHECK_BASE_OF_COMMAND(TCommand)
		DYNAMIC_CMD_CHECK_ARRAY_FIELD(TCommand)

		PreProcessElement(PacketType::Command);

		const int cmdSize = TCommand::_Size(_count);
#if _DEBUG
		if (cmdSize <= 0 || cmdSize >= 2500)
		{
			jc_assert_msg(false, "%s::_Size(%d) = %d 커맨드 사이즈가 이상합니다.", TCommand::_Name(), _count, cmdSize);
		}
#endif
		TCommand* pCmd = (TCommand*)(buffer_ + writePos_);
		TCommand::_Construct(pCmd, _count);

		if (!MoveWritePos(cmdSize))
		{
			jc_assert_msg(false, "버퍼에 커맨드를 쓸 공간이 부족합니다.");
		}

		PacketHeader& header = GetPacketHeader();
		++header.elemCount_;
		header.payloadLen_ += cmdSize;
		return *pCmd;
	}

	static jc::SharedPtr<PacketBuffer> Create(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize = 6000);

	bool			IsValid() const;
	
	ElemCnt_t		GetElemCount();
	PaloadLen_t		GetPayloadLength();

private:
	bool			PreProcessElement(PacketType_t _type);
	PacketHeader&	GetPacketHeader() const { return *reinterpret_cast<PacketHeader*>(buffer_ + currPacketHeaderOffset_); }
	PacketHeader*	GetPacketHeaderPtr() const { return reinterpret_cast<PacketHeader*>(buffer_ + currPacketHeaderOffset_); }

private:
	bool			currMsgPending_ = false;		// 현재 패킷 버퍼에 완성되지 않은 메시지가 존재하는지 여부
	int				currMsgStartOffset_ = 0;	// 현재 패킷 버퍼에 가장 마지막으로 추가된 요소의 시작 오프셋값
	int				currPacketHeaderOffset_ = 0;
};

using PacketBufferPtr = jc::SharedPtr<PacketBuffer>;

NS_END
