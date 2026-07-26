/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "VirtualMachine.h"

#include "jc/Logger/LoggerDefine.h"
#include <cstdio>
#include "jc/Threading/Scheduler.h"

#include "sg/Core/AppConfig.h"
#include "sg/Core/CLIThread.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
VirtualMachine::VirtualMachine()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
VirtualMachine::~VirtualMachine()
{
	if (pThreadPool_)
	{
		pThreadPool_->Join(ThreadPool::JoinStrategy::WaitAllTasks);
	}

	if (pScheduler_)
	{
		pScheduler_->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);
	}

	if (pCliThread_)
	{
		pCliThread_->SendInterrupt();
		pCliThread_->Join();
	}

	JC_DELETE_SAFE(pScheduler_);
	JC_DELETE_SAFE(pThreadPool_);
	JC_DELETE_SAFE(pCliThread_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void VirtualMachine::Init()
{
	_LogDebug_("[DBG] VM: new CLIListener");
	pCLIListener_ = dbg_new CLIListener;
	_LogDebug_("[DBG] VM: new CLIThread");
	pCliThread_ = dbg_new CLIThread;
	_LogDebug_("[DBG] VM: SetListener");
	pCliThread_->SetListener(pCLIListener_);
	_LogDebug_("[DBG] VM: new ThreadPool");
	pThreadPool_ = dbg_new jc::ThreadPool(2);
	_LogDebug_("[DBG] VM: new Scheduler");
	pScheduler_ = dbg_new jc::Scheduler(2);
	_LogDebug_("[DBG] VM: Start");
	pCliThread_->Start();
	printf("[RAW] Init done\n"); fflush(stdout);
}

//////////////////////////////////////////////////////////////////////////////////////////
void VirtualMachine::Go(const jc::TimeSpan& _dt)
{
	pCliThread_->ProcessInputs();
}
