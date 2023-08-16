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
		SGVector<SGString> Arguments;

		Input();
		Input(SGVector<SGString>&& args);
		Input(const Input& other) = delete;
		Input(Input&& other) noexcept;
		~Input();

		Input& operator=(const Input& other) = delete;
		Input& operator=(Input&& other) noexcept;
	};

	CLIThread();
	~CLIThread() override;

	// 입력을 수행하고 싶은 쓰레드에서 호출할 것
	void SetListener(ICLIListener* listener);
	void ProcessInputs();
	void SendInterrupt();
protected:
	JCore::NormalLock m_Lock;
	volatile bool m_bRunning;
	JCore::AtomicBool m_bHasInput;
	ICLIListener* m_pListener;
	JCore::ArrayQueue<Input> m_qInputs;
	int m_iMaxInputEventCount;
};





