/*
	작성자 : 윤정도
	C#의 event 모방
*/

#pragma once

#include "jc/Functional.h"
#include "jc/TypeTraits.h"
#include "jc/Container/Vector.h"

NS_JC_BEGIN

template <typename... Args>
class Event
{
	using TAction = Action<Args...>;

	struct Holder
	{
		int id_;
		TAction action_;

		template <typename TInvoker>
		Holder(int _id, TInvoker&& _fn)
			: id_(_id)
			, action_(Forward<TInvoker>(_fn))
		{}

		template <typename... ParamArgs>
		void Invoke(ParamArgs&&... _args) {
			this->action_.operator()(Forward<ParamArgs>(_args)...);
		}

		const type_info& TargetType() { return this->action_.target_type(); }
	};
public:
	Event(int _capacity = 2) : m_Chain(_capacity) {}
	~Event() { Clear(); }

	bool Register(int _id, const TAction& _fn) {
		if (IsRegistered(_id)) {
			return false;
		}

		m_Chain.PushBack({ _id, _fn });
		return true;
	}

	bool Register(int _id, TAction&& _fn) {
		if (IsRegistered(_id)) {
			return false;
		}

		m_Chain.PushBack({ _id, Move(_fn) });
		return true;
	}

	bool Unregister(int _id) {
		for (int i = 0; i < m_Chain.Size(); ++i) {
			if (m_Chain[i].id_ == _id) {
				m_Chain.RemoveAt(i);
				return true;
			}
		}

		return false;
	}

	bool IsRegistered(int _id) const {
		for (int i = 0; i < m_Chain.Size(); ++i) {
			if (m_Chain[i].id_ == _id) {
				return true;
			}
		}

		return false;
	}

	void Clear() {
		m_Chain.Clear();
	}

	template <typename... ParamArgs>
	void Invoke(ParamArgs&&... _params) {
		for (int i = 0; i < m_Chain.Size(); ++i) {
			m_Chain[i].action_(Forward<ParamArgs>(_params)...);
		}
		// m_Chain.ForEach([&params...](Holder* holder) {
		// 	  holder->Invoke(Forward<_Args>(params)...);
		// });
	}

	int Size() const {
		return m_Chain.Size();
	}

	template <typename... ParamArgs>
	void operator()(ParamArgs&&... _args) {
		Invoke(Forward<ParamArgs>(_args)...);
	}
private:
	Vector<Holder> m_Chain;
};



NS_END