/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Assert.h>
#include <JCore/Primitives/Atomic.h>

NS_JC_BEGIN

struct RefCountObject
{
	RefCountObject() : m_iRef(1) {}
	virtual ~RefCountObject() = default;

	void AddRef() { ++m_iRef; }
	void AddRef(int cnt) { m_iRef += cnt; }
	int GetRefCount() const { return m_iRef.Load(); }

	// 계산을 똑바로해서 Release 호출하도록 할 것
	// m_iRef가 0이하가 되는 순간이 2회 이상인 경우가 되면 안됨.
	void Release(int cnt = 1);
protected:
    virtual void ReleaseAction() {}
private:
    Atomic<int> m_iRef;
};

NS_JC_END