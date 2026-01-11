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

	jc::ThreadPool&		GetThreadPool() const { return *pThreadPool_; }
	jc::Scheduler&		GetScheduler() const { return *pScheduler_; }
	CLIListener&		GetCLIListener() const { return *pCLIListener_; }

private:
	CLIListener*	 pCLIListener_ = nullptr;
	CLIThread*		 pCliThread_ = nullptr;
	jc::ThreadPool*	 pThreadPool_ = nullptr;
	jc::Scheduler*	 pScheduler_ = nullptr;
};

JC_DECL_SINGLETON_VAR(VirtualMachine)
#define g_cVM JC_DECL_SINGLETON_BODY(VirtualMachine)
#define g_cCLIListener (g_cVM.GetCLIListener())
#define g_cThreadPool  (g_cVM.GetThreadPool())
#define g_cScheduler   (g_cVM.GetScheduler())


