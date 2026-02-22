/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jc/Container/Vector.h>

#include <jnet/Host/Session.h>

NS_JNET_BEGIN

class SessionContainer
{
public:
	SessionContainer(int _capacity);
	~SessionContainer();

	void		SetInitialHandleSeq(int _initialHandleSeq) { initialHandleSeq_ = _initialHandleSeq; }
	int			GetInitialHandleSeq() const { return initialHandleSeq_; }
	int			CreateHandle();
	void		ResetHandleSeq() { handleSeq_ = 0; }
	int			Capacity() { return sessionList_.Capacity(); }
	bool		Add(Session* _pSession);
	int			Size() const { return size_; }
	Session*	Get(int _handle);
	void		DisconnectAll();
	void		Clear();
	void		ForEach(jc::Action<Session*> _fn);
	void		ForEachConnected(jc::Action<Session*> _fn);
	bool		IsValidHandle(int _handleIndex);

protected:
	int handleSeq_;
	int initialHandleSeq_;
	int size_;
	jc::Vector<Session*> sessionList_; // TODO: StaticVector
};

NS_END
