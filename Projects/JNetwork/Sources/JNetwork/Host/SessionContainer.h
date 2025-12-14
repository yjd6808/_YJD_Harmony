/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/Vector.h>

#include <JNetwork/Host/ISessionContainer.h>
#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN
class SessionContainer : public ISessionContainer
{
public:
	using ContainerType = JCore::Vector<Session*>;

	SessionContainer(int _capacity);
	~SessionContainer() override;

	void SetInitialHandleSeq(int _initialHandleSeq) override
	{
		initialHandleSeq_ = _initialHandleSeq;
	}

	int GetInitialHandleSeq() const override
	{
		return initialHandleSeq_;
	}

	int CreateHandle() override;

	void ResetHandleSeq() override
	{
		handleSeq_ = 0;
	}

	int Capacity() override
	{
		return sessionList_.Capacity();
	}

	bool Add(Session* _pSession) override;

	int Size() override
	{
		return size_;
	}

	Session* Get(int _handle) override;
	bool Remove(int _handle) override;
	void DisconnectAll() override;
	void Clear() override;
	void ForEach(JCore::Action<Session*> _fn) override;
	void ForEachConnected(JCore::Action<Session*> _fn) override;
	bool IsValidHandle(int _handleIndex) override;

protected:
	int handleSeq_;
	int initialHandleSeq_;
	int size_;
	ContainerType sessionList_; // TODO: StaticVector
};

NS_JNET_END
