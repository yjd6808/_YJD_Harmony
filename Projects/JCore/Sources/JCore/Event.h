/*
	작성자 : 윤정도
	C#의 event 모방
*/

#pragma once

#include <JCore/Functional.h>
#include <JCore/TypeTraits.h>
#include <JCore/Container/HashMap.h>

NS_JC_BEGIN

template <typename... Args>
class Event
{
	using TEvent  = Event<Args...>;
	using TAction = Action<Args...>;

	struct Holder
	{
		TAction Action;

		Holder(const TAction& fn) : Action(fn) {}
		Holder(TAction&& fn) : Action(Move(fn)) {}

		void Invoke(Args&&... args) { this->Action(Forward<Args>(args)...); }
		const type_info& TargetType() { return this->Action.target_type(); }
	};
public:
	~Event() { Clear(); }

	template <typename TInvoker>
	bool Register(int id, TInvoker&& fn) {
		static_assert(IsCallable_v<TInvoker>, "... TInvoker is not callable");
		if (m_Chain.Exist(id)) {
			return false;
		}

		return m_Chain.Insert(id, dbg_new Holder{ Forward<TInvoker>(fn) });
	}

	bool Unregister(int id) {
		Holder** ppFind = m_Chain.Find(id);

		if (ppFind == nullptr) {
			return false;
		}

		delete (*ppFind);
		return m_Chain.Remove(id);
	}

	void Clear() {
		m_Chain.ForEachValueDelete();
	}

	void Invoke(Args&&... params) {
		m_Chain.ForEachValue([&params...](Holder* holder) {
			holder->Invoke(Forward<Args>(params)...);
		});
	}

	int Size() const {
		return m_Chain.Size();
	}

	void operator()(Args&&... args) {
		Invoke(Forward<Args>(args)...);
	}
private:
	HashMap<int, Holder*> m_Chain;
};



NS_JC_END