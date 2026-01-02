/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:30:33 AM
 * =====================
 */


#pragma once

#include <jc/Pool/MemoryPoolAbstract.h>

#include <jnet/Buffer/BufferAbstract.h>
#include <jnet/Packet/Command.h>


NS_JNET_BEGIN

class CommandBuffer : public BufferAbstract
{
public:
	CommandBuffer(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize);
	CommandBuffer(const CommandBuffer& _other);
	~CommandBuffer() override;

	template <typename TCommand>
	TCommand& Alloc()
	{
		static_assert(jc::IsBaseOf_v<StaticCommand, TCommand>, "... this method is only for StaticCommand ");
		return Alloc<TCommand>(0);
	}

	template <typename TCommand>
	TCommand& Alloc(int _count)
	{
		CMD_CHECK_BASE_OF_COMMAND(TCommand)
		DYNAMIC_CMD_CHECK_ZERO_SIZE_ARRAY_FIELD(TCommand)

		const int cmdSize = TCommand::_Size(_count);
#if _DEBUG
		if (cmdSize <= 0 || cmdSize >= 2500)
		{
			jc_assert_msg(false, "%s::_Size(%d) = %d 커맨드 사이즈가 이상합니다.", TCommand::_Name(), _count, cmdSize);
		}
#endif

		if (!MoveWritePos(cmdSize))
		{
			jc_assert_msg(false, "버퍼에 커맨드를 쓸 공간이 부족합니다.");
		}

		jc_assert_msg(readPos_ + cmdSize == writePos_, "리드포스 또는 라이트포스가 이상합니다.");
		TCommand* pCmd = Peek<TCommand*>();
		TCommand::_Construct(pCmd, _count);

		MoveReadPos(cmdSize);
		AddCommandCount();
		AddPacketLength(cmdSize);
		return *pCmd;
	}

	void Alloc(ICommand* _pCmd);

	static jc::SharedPtr<CommandBuffer> Create(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize = 6000);

	void Initialize();
	void AddCommandCount();
	void AddPacketLength(int _size);
	bool IsValid() const;
	int GetBufferRequestSize()
	{
		return requestBufferSize_;
	}
	CmdCnt_t GetCommandCount();
	PktLen_t GetPacketLength();

private:
	int requestBufferSize_;
	jc::MemoryPoolAbstractPtr allocator_;
};

using CommandBufferPtr = jc::SharedPtr<CommandBuffer>;

NS_JNET_END
