/*
 * 작성자: 윤정도
 * 생성일: 4/15/2023 6:21:23 AM
 * =====================
 *
 */


#pragma once


#include "jc/Sync/NormalLock.h"
#include "jc/Sync/AutoResetEvent.h"

#include "jnet/IOCP/IOCPTaskState.h"

NS_JNET_BEGIN

class IOCP;
class JC_NOVTABLE IOCPTaskAbstract : public jc::MakeSharedFromThis<IOCPTaskAbstract>
{
public:
	IOCPTaskAbstract(IOCP* _pIocp)
	: waitHandle_(false)
	, state_(IOCPTaskState::eInitialized)
	, iocp_(_pIocp)
	, id_(0)
	, type_(0)
	, pContinuousTaskLock_(nullptr)
	{
	}

	virtual ~IOCPTaskAbstract() = default;
	virtual void Start() = 0;

	virtual void Call() = 0;
	virtual void CallFinally() = 0;
	virtual void OnFailed(_u32 _errorCode) = 0;

	bool IsReady()
	{
		return state_ >= IOCPTaskState::eReady;
	}

	int GetState()
	{
		return state_;
	}

	void SetOwnerId(int _ownerId) { ownerId_ = _ownerId; }
	int GetOwnerId() const { return ownerId_; }

	void SetId(int _id) { id_ = _id; }
	int GetId() const { return id_; }

	void SetType(int _type) { type_ = _type; }
	int GetType() const { return type_; }

protected:
	jc::AutoResetEvent waitHandle_;
	jc::AtomicInt state_;

	IOCP* iocp_;

	int ownerId_ = -1;
	int id_;
	int type_;

	jc::SharedPtr<IOCPTaskAbstract> pContinuousTask_;	// 이게 존재하면 무조건 존재한다. 그게 아니면 그냥 서버터지삼
	jc::NormalLock* pContinuousTaskLock_;

	friend class IOCPOverlappedTask;
};

using IOCPTaskAbstractPtr = jc::SharedPtr<IOCPTaskAbstract>;

NS_END
