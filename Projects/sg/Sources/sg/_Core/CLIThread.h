/*
	작성자 : 윤정도
	
	입력받기 전용 쓰레드
*/

#pragma once

#include "jc/Threading/RunnableThread.h"
#include "jc/Container/ArrayQueue.h"

#include "sg/_Core/CLIListener.h"

class CLIThread : public jc::RunnableThread
{
protected:
	bool PreStart() override;
	bool PreStop() override;
	void WorkerThread() override;

public:
	struct Input
	{
		jc::Vector<jc::String> arguments_;

		Input();
		Input(jc::Vector<jc::String>&& _arguments);
		Input(const Input& _other) = delete;
		Input(Input&& _other) noexcept;
		~Input();

		Input& operator=(const Input& _other) = delete;
		Input& operator=(Input&& _other) noexcept;
	};

	CLIThread();
	~CLIThread() override;

	// 입력을 수행하고 싶은 쓰레드에서 호출할 것
	void SetListener(CLIListener* _pListener);
	void ProcessInputs();
	void SendInterrupt();

protected:
	jc::NormalLock lock_;
	volatile bool isRunning_;
	jc::AtomicBool hasInput_;
	CLIListener* pListener_;
	jc::ArrayQueue<Input> inputQueue_;
	int maxInputEventCount_;
};
