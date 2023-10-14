/*
 * 작성자: 윤정도
 * 생성일: 8/2/2023 6:21:12 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

class Session;
class ISessionContainer
{
public:
	virtual ~ISessionContainer() = default;

	virtual void SetInitialHandleSeq(int initialHandleSeq) = 0;
	virtual int GetInitialHandleSeq() const = 0;
	virtual int CreateHandle() = 0;
	virtual void ResetHandleSeq() = 0;

	virtual int Capacity() = 0;
	virtual int Size() = 0;
	virtual bool Add(Session* session) = 0;
	virtual Session* Get(int handle) = 0;
	virtual bool Remove(int handle) = 0;
	virtual void ForEach(JCore::Action<Session*> action) = 0;
	virtual void ForEachConnected(JCore::Action<Session*> action) = 0;
	virtual void DisconnectAll() = 0;
	virtual void Clear() = 0;
	virtual bool IsValidHandle(int handleIndex) = 0;
};

NS_JNET_END