/*
	작성자 : 윤정도
*/


#pragma once

#include <JCore/Container/Vector.h>

#include <JNetwork/Host/ISessionContainer.h>
#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN

class SessionContainer : public ISessionContainer
{
public:
	using TContainer = JCore::Vector<Session*>;

	SessionContainer(int capacity);
	~SessionContainer() override;

	void SetInitialHandleSeq(int initialHandleSeq) override { m_iInitialHandleSeq = initialHandleSeq; }
	int GetInitialHandleSeq() const override { return m_iInitialHandleSeq; }
	int CreateHandle() override;
	void ResetHandleSeq() override { m_iHandleSeq = 0; }

	int Capacity() override { return m_vSessionList.Capacity(); }
	bool Add(Session* session) override;
	int Size() override { return m_iSize; }
	Session* Get(int handle) override;
	bool Remove(int handle) override;
	void DisconnectAll() override;
	void Clear() override;
	void ForEach(JCore::Action<Session*> fn) override;
	void ForEachConnected(JCore::Action<Session*> fn) override;
	bool IsValidHandle(int handleIndex) override;
protected:
	int m_iHandleSeq;
	int m_iInitialHandleSeq;
	int m_iSize;
	TContainer m_vSessionList; // TODO: StaticVector
};

NS_JNET_END