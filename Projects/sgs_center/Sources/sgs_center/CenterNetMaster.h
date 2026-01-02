/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <sgs/CommonNetGroupMgr.h>

class CenterNetMaster final
	: public CommonNetGroupMgr
	, public jc::SingletonPointer<CenterNetMaster>
{
private:
	friend class TSingleton;
	CenterNetMaster();
	~CenterNetMaster() override;
public:
	void Initialize() override;
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Center; }
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
	void OnStopped() override;
};



