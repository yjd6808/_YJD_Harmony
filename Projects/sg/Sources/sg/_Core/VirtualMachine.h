// 현재 프로세스의 컨텍스트를 나타냄
// 스크립트 수행등 수행

#pragma once

class CLIThread;
struct RuntimeConfig;
struct CharCommonInfo;
class DescLoaderMgr;

class jc::ThreadPool;
class jc::Scheduler;
class jnet::CommandNameDictionary;

class VirtualMachine : public jc::SingletonPointer<VirtualMachine>
{
public:
	VirtualMachine();
	~VirtualMachine();

	void Init();
	void Go(const jc::TimeSpan& _dt);

	jc::ThreadPool& ThreadPool() const { return *pThreadPool_; }
	jc::Scheduler&	Scheduler() const { return *pScheduler_; }
	jnet::CommandNameDictionary& CommandNameDictionary() const { return *pCmdNameMap_; }

private:
	CLIThread*		 pCliThread_ = nullptr;
	RuntimeConfig*	 pConfigRuntime_ = nullptr;
	CharCommonInfo*  pCharCommonInfo_ = nullptr;
	jc::ThreadPool*	 pThreadPool_ = nullptr;
	jc::Scheduler*	 pScheduler_ = nullptr;
	jnet::CommandNameDictionary*	pCmdNameMap_ = nullptr;
};

#define g_cVM (*VirtualMachine::Get())
