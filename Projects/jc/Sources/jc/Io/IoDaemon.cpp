#include "jc/IO/IODaemon.h"
#include "jc/Assert.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
IODaemon::IODaemon()
	: files_(resolver_, engine_)
	, http_(resolver_, engine_)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
IODaemon::~IODaemon()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IODaemon::Initialize(const IODaemonConfig& _config)
{
	jc_assert(!initialized_);

	IOJobEngineConfig engineConfig;
	engineConfig.workerCount_ = _config.workerCount_;
	engineConfig.readUnit_ = _config.readUnit_;
	engineConfig.progressNotifyIntervalMs_ = _config.progressNotifyIntervalMs_;
	engineConfig.completedProgressKeep_ = _config.completedProgressKeep_;
	engineConfig.pSharedPool_ = _config.pSharedPool_;

	if (!engine_.Initialize(engineConfig))
		return false;

	if (!http_.Initialize(_config.http_, _config.memoryLimit_, _config.readUnit_))
	{
		engine_.Shutdown();
		return false;
	}

	files_.InitializeDefaults(_config.memoryLimit_, _config.readUnit_);
	initialized_ = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IODaemon::Shutdown()
{
	if (!initialized_)
		return;

	http_.Shutdown();
	engine_.Shutdown();
	initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IODaemon::SetListener(IIOListener* _pListener, bool _transferOwnership)
{
	engine_.SetListener(_pListener, _transferOwnership);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IODaemon::Update()
{
	engine_.Pump();
}

NS_END
