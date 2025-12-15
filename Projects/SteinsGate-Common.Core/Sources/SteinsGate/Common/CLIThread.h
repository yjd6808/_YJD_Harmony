/*
	작성자 : 윤정도
	
	입력받기 전용 쓰레드
*/

#pragma once

#include <JCore/Threading/RunnableThread.h>
#include <JCore/Container/ArrayQueue.h>

#include <SteinsGate/Common/ICLIListener.h>

class CLIThread : public JCore::RunnableThread
{
protected:
	bool PreStart() override;
	bool PreStop() override;
	void WorkerThread() override;

public:
	struct Input
	{
		SGVector<SGString> arguments_;

		Input();
		Input(SGVector<SGString>&& _arguments);
		Input(const Input& _other) = delete;
		Input(Input&& _other) noexcept;
		~Input();

		Input& operator=(const Input& _other) = delete;
		Input& operator=(Input&& _other) noexcept;
	};

	CLIThread();
	~CLIThread() override;

	// 입력을 수행하고 싶은 쓰레드에서 호출할 것
	void SetListener(ICLIListener* _pListener);
	void ProcessInputs();
	void SendInterrupt();

protected:
	JCore::NormalLock lock_;
	volatile bool isRunning_;
	JCore::AtomicBool hasInput_;
	ICLIListener* pListener_;
	JCore::ArrayQueue<Input> inputQueue_;
	int maxInputEventCount_;
};
