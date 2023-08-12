/*
	작성자 : 윤정도
	C#의 event 모방
*/

#pragma once

#include <JCore/Functional.h>
#include <JCore/TypeTraits.h>
#include <JCore/Container/Vector.h>

NS_JC_BEGIN

template <typename... Args>
class Event
{
	using TAction = Action<Args...>;

	struct Holder
	{
		int ID;
		TAction Action;

		template <typename TInvoker>
		Holder(int id, TInvoker&& fn)
			: ID(id)
			, Action(Forward<TInvoker>(fn))
		{}

		template <typename... _Args>
		void Invoke(_Args&&... args) {
			this->Action.operator()(Forward<_Args>(args)...);
		}

		const type_info& TargetType() { return this->Action.target_type(); }
	};
public:
	Event(int capacity = 2) : m_Chain(capacity) {}
	~Event() { Clear(); }

	bool Register(int id, const TAction& fn) {
		if (IsRegistered(id)) {
			return false;
		}

		m_Chain.PushBack({ id, fn });
		return true;
	}

	bool Register(int id, TAction&& fn) {
		if (IsRegistered(id)) {
			return false;
		}

		m_Chain.PushBack({ id, Move(fn) });
		return true;
	}

	bool Unregister(int id) {
		for (int i = 0; i < m_Chain.Size(); ++i) {
			if (m_Chain[i].ID == id) {
				m_Chain.RemoveAt(i);
				return true;
			}
		}

		return false;
	}

	bool IsRegistered(int id) const {
		for (int i = 0; i < m_Chain.Size(); ++i) {
			if (m_Chain[i].ID == id) {
				return true;
			}
		}

		return false;
	}

	void Clear() {
		m_Chain.Clear();
	}

	template <typename... _Args>
	void Invoke(_Args&&... params) {
		for (int i = 0; i < m_Chain.Size(); ++i) {
			m_Chain[i].Action(Forward<_Args>(params)...);
		}
		// m_Chain.ForEach([&params...](Holder* holder) {
		// 	  holder->Invoke(Forward<_Args>(params)...);
		// });
	}

	int Size() const {
		return m_Chain.Size();
	}

	template <typename... _Args>
	void operator()(_Args&&... args) {
		Invoke(Forward<_Args>(args)...);
	}
private:
	Vector<Holder> m_Chain;
};



NS_JC_END