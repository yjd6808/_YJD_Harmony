/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Core.h"
#include "ServerCoreHeader.h"
#include "CommonNetGroupMgr.h"

#include <sgs/CommonNetGroup.h>
#include <sgs/InterServerClientNetGroup.h>
#include <sgs/InterServerSendHelper.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
CommonNetGroupMgr::CommonNetGroupMgr(int _updatePerSecond)
: updatePerSecond_(_updatePerSecond)
, running_(true)
, processInfo_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroupMgr::Initialize()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroupMgr::SetProcessInfo(ServerProcessInfo* _pProcessInfo)
{
	if (processInfo_)
	{
		_LogWarn_("이미 주입된 정보가 있습니다.");
		return;
	}

	processInfo_ = _pProcessInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroupMgr::ProcessMainUpdate()
{
	PulserStatistics pulseStat;
	Pulser pulser(1000 / updatePerSecond_, Pulser::eSliceCycle, &pulseStat);
	TimeSpan elapsed;

	pulser.Start();
	while (running_)
	{
		if (sg::CLIThread)
			sg::CLIThread->ProcessInputs();

		ProcessSubUpdate(elapsed);
		OnUpdate(elapsed);

		elapsed = pulser.Wait();
		// Console::WriteLine("%d", elapsed.GetTotalMiliSecondsInt());
	}

	Finalize();
	OnStopped();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetGroupMgr::ProcessSubUpdate(const TimeSpan& _elapsed)
{
	if (sg::CommonNetGroup)
		sg::CommonNetGroup->ProcessUpdate(_elapsed);

	if (sg::InterServerClientNetGroup)
		sg::InterServerClientNetGroup->ProcessUpdate(_elapsed);
}
