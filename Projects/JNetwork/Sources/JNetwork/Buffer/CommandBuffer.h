/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:30:33 AM
 * =====================
 */


#pragma once

#include <JCore/Pool/MemoryPoolAbstract.h>

#include <JNetwork/Buffer/BufferAbstract.h>
#include <JNetwork/Packet/Command.h>


NS_JNET_BEGIN

class CommandBuffer : public BufferAbstract
{
public:
	CommandBuffer(const JCore::MemoryPoolAbstractPtr& allocator, int bufferSize);
	CommandBuffer(const CommandBuffer& other);
	~CommandBuffer();

	template <typename TCommand, typename... Args>
	TCommand& Alloc(Args&&... args) {
		static_assert(JCore::IsBaseOf_v<ICommand, TCommand>, "... TCommand is not command [1]");

		static constexpr int CmdSize = sizeof(TCommand);

		if (MoveWritePos(sizeof(TCommand)) == false) {
			DebugAssertMsg(false, "버퍼에 커맨드를 쓸 공간이 부족합니다.");
		}

		DebugAssertMsg(m_iReadPos + CmdSize == m_iWritePos, "리드포스 또는 라이트포스가 이상합니다.");
		TCommand* cmd = Read<TCommand*>();
		JCore::Memory::PlacementNew(cmd, JCore::Forward<Args>(args)...);

		AddCommandCount();
		AddPacketLength(CmdSize);
		return *cmd;
	}

	void Initialize();
	void AddCommandCount();
	void AddPacketLength(int size);
	bool IsValid();
	int GetBufferRequestSize() { return m_iRequestBufferSize; }
	CmdCnt_t GetCommandCount();
	PktLen_t GetPacketLength();
private:
	int m_iRequestBufferSize;
	JCore::MemoryPoolAbstractPtr m_Allocator;
};

using CommandBufferPtr = JCore::SharedPtr<CommandBuffer>;

NS_JNET_END