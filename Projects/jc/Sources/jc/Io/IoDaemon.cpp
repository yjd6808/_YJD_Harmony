#include "jc/Io/IoDaemon.h"
#include "jc/Assert.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
IoDaemon::IoDaemon()
	: files_(resolver_, engine_)
	, http_(resolver_, engine_)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
IoDaemon::~IoDaemon()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IoDaemon::Initialize(const IoDaemonConfig& _config)
{
	jc_assert(!initialized_);

	IoJobEngineConfig engineConfig;
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
void IoDaemon::Shutdown()
{
	if (!initialized_)
		return;

	http_.Shutdown();
	engine_.Shutdown();
	initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IoDaemon::SetListener(IIoListener* _pListener, bool _transferOwnership)
{
	engine_.SetListener(_pListener, _transferOwnership);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IoDaemon::Update()
{
	engine_.Pump();
}

NS_END
