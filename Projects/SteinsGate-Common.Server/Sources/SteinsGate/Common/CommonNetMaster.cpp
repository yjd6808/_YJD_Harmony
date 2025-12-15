/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonNetMaster.h"

#include <SteinsGate/Common/CommonNetGroup.h>
#include <SteinsGate/Common/InterServerClientNetGroup.h>
#include <SteinsGate/Common/InterServerSendHelper.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
CommonNetMaster::CommonNetMaster(int _updatePerSecond)
: updatePerSecond_(_updatePerSecond)
, running_(true)
, processInfo_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetMaster::Initialize()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetMaster::SetProcessInfo(ServerProcessInfo* _pProcessInfo)
{
	if (processInfo_)
	{
		_LogWarn_("이미 주입된 정보가 있습니다.");
		return;
	}

	processInfo_ = _pProcessInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetMaster::ProcessMainUpdate()
{
	PulserStatistics pulseStat;
	Pulser pulser(1000 / updatePerSecond_, Pulser::eSliceCycle, &pulseStat);
	TimeSpan elapsed;

	pulser.Start();
	while (running_)
	{
		if (Core::CLIThread)
			Core::CLIThread->ProcessInputs();

		ProcessSubUpdate(elapsed);
		OnUpdate(elapsed);

		elapsed = pulser.Wait();
		// Console::WriteLine("%d", elapsed.GetTotalMiliSecondsInt());
	}

	Finalize();
	OnStopped();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommonNetMaster::ProcessSubUpdate(const TimeSpan& _elapsed)
{
	if (Core::CommonNetGroup)
		Core::CommonNetGroup->ProcessUpdate(_elapsed);

	if (Core::InterServerClientNetGroup)
		Core::InterServerClientNetGroup->ProcessUpdate(_elapsed);
}
