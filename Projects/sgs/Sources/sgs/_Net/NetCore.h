/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */

#pragma once

#include <jnet/NetGroupMgr.h>

struct ServerProcessInfo;

NS_SG_BEGIN

class NetCore : public jnet::NetGroupMgr, public jc::SingletonPointer<NetCore>
{
public:
	NetCore();

	bool Initialize() override;

	int GetProcessType() { return processType_; }
	const ServerProcessInfo& GetProcessInfo() { return *pProcessInfo_; }
	const NetInterServerInfo& GetInterServerInfo() const { return pProcessInfo_->GetInterServerInfo(); }
	const jc::Vector<NetServerInfo>& GetMainServerInfoList() const { return pProcessInfo_->mainServerInfoList_; }

	void ProcessMainLoop();
	void Terminate() { running_ = false; }

	void SetUpdateCallback(const jc::Action<const jc::TimeSpan&>& _fnUpdate) { fnUpdate_ = _fnUpdate; }
	void SetStoppedCallback(const jc::Action<>& _fnStopped) { fnStopped_ = _fnStopped; }

	void SetUpdateCallback(jc::Action<const jc::TimeSpan&>&& _fnUpdate) { fnUpdate_ = std::move(_fnUpdate); }
	void SetStoppedCallback(jc::Action<>&& _fnStopped) { fnStopped_ = std::move(_fnStopped); }

	void	UpdateServerTime(Int64 _sendTime, Int64 _centerTime);
	Int64	GetTime();
	Int64	GetTimeDiff() const { return centerServerTimeDiff_; }
private:
	void OnUpdate(const jc::TimeSpan& _elapsed);

private:
	int processType_ = 0;
	int updatePerSecond_;
	bool running_;
	bool isInterServerClient_ = false;
	ServerProcessInfo* pProcessInfo_ = nullptr;

	jc::Action<const jc::TimeSpan&> fnUpdate_;
	jc::Action<> fnStopped_;

	// -----------------------------------------------
	// 동적인 변수
	// 모든 서버의 시간이 동일하기 위해서, 중앙서버의 시간을 기준으로 피어 서버들이 시간 동기화를 한다.
	// 피어 서버들이 중앙 서버와 시간 정보를 기록한다.
	// 매번 정확한 시간 정보를 얻기 위해 중앙 서버에 시간정보를 물어보는건 엄청난 낭비이다.
	Int64 centerServerPrevTime_ = -1;
	Int64 centerServerTime_ = -1;
	Int64 centerServerTimeDiff_ = -1; // 피어 서버와 중앙 서버의 시간 차이
	static constexpr int RTT_QUEUE_MAX_SIZE = 10;
	Int64 rttQueue_[RTT_QUEUE_MAX_SIZE];
	Int32 rttQueueIndex_ = 0;
	Int32 rttQueueSize_ = 0;
	Int64 rttSum_ = 0;
	Int64 avgRtt_ = 0;
};

NS_END

#define g_cNetCore JC_DECL_SINGLETON_BODY(sg::NetCore)