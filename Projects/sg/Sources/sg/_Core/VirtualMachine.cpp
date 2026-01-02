/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Core.h"
#include "VirtualMachine.h"

#include <jc/Threading/Scheduler.h>

#include <sg/Config.h>
#include <sg/CLIThread.h>

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

	JC_DELETE_SAFE(pCmdNameMap_);
	JC_DELETE_SAFE(pScheduler_);
	JC_DELETE_SAFE(pThreadPool_);
	JC_DELETE_SAFE(pCliThread_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void VirtualMachine::Init()
{
	pCliThread_ = dbg_new CLIThread;
	pCliThread_->SetListener(dbg_new CLIListener);
	pCliThread_->Start();
	pThreadPool_ = dbg_new jc::ThreadPool(2);
	pScheduler_ = dbg_new jc::Scheduler(2);
	pCmdNameMap_ = dbg_new jnet::CommandNameDictionary;
}

//////////////////////////////////////////////////////////////////////////////////////////
void VirtualMachine::Go(const jc::TimeSpan& _dt)
{
	pCliThread_->ProcessInputs();
}
