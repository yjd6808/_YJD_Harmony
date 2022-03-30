/*
	작성자 : 윤정도
*/

#pragma once

#include <atomic>

namespace JCore {

struct RefCount
{
	RefCount() = default;
	virtual ~RefCount() = default;

	void AddRef()				{ m_Ref++; }
	void AddRef(int cnt)		{ m_Ref += cnt; }
	int  GetRefCount() const	{ return m_Ref; }

	virtual void Release() {
		m_Ref--;

		if (m_Ref <= 0) {
			delete this;
		}
	}

protected:
	std::atomic<int> m_Ref = 0;
};

} // namespace JCore