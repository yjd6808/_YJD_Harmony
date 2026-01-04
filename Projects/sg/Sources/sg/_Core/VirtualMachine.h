// 현재 프로세스의 컨텍스트를 나타냄
// 스크립트 수행등 수행

#pragma once

class CLIThread;
class CLIListener;
class jc::ThreadPool;
class jc::Scheduler;

class VirtualMachine : public jc::SingletonPointer<VirtualMachine>
{
public:
	VirtualMachine();
	~VirtualMachine();

	void Init();
	void Go(const jc::TimeSpan& _dt);

	void AddCLICallback(const jc::String& _cmdLine, const jc::Func<bool, int, jc::String*>& _callback);

	jc::ThreadPool& ThreadPool() const { return *pThreadPool_; }
	jc::Scheduler&	Scheduler() const { return *pScheduler_; }

private:
	CLIListener*	 pCLIListener_ = nullptr;
	CLIThread*		 pCliThread_ = nullptr;
	jc::ThreadPool*	 pThreadPool_ = nullptr;
	jc::Scheduler*	 pScheduler_ = nullptr;
};

#define g_cVM (*VirtualMachine::Get())
