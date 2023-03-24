/*
	작성자 : 윤정도
	C#의 event 모방
*/

#pragma once

#include <JCore/Functional.h>
#include <JCore/Container/LinkedList.h>

NS_JC_BEGIN

template <typename... Args>
class Event
{
private:
	using TEvent  = Event<Args...>;
	using TAction = Action<Args...>;
private:
	
	struct Callback
	{
		void*	FnPointer = nullptr;
		TAction Action;

		Callback() {}
		Callback(const TAction& fn, void* fnptr) {
			this->Action = fn; 
			this->FnPointer = fnptr;
		}

		void Invoke(Args&&... args) {
			this->Action(Forward<Args>(args)...);
		}

		const type_info& TargetType() {
			return this->Action.target_type();
		}
	};

public:

	template <typename TInvoker>
	void Register(const TInvoker& fn) {
		m_MethodChain.PushBack({fn, (void*)AddressOf(fn)});
	}

	template <typename TInvoker>
	bool Unregister(const TInvoker& fn) {
		return m_MethodChain.RemoveIf([&fn](Callback& call) {
			return AddressOf(fn) == call.FnPointer;
		});
	}

	bool UnregisterByType(const type_info& fnType) {
		return m_MethodChain.RemoveIf([&fnType](Callback& call) {
			return fnType == call.TargetType();
		});
	}

	void Clear() {
		m_MethodChain.Clear();
	}

	// 복사 버라이어딕 주의
	void Invoke(Args... params) {
		m_MethodChain.Extension().ForEach([&params...](Callback& fn) {
			fn.Invoke(Forward<Args>(params)...);
		});
	}

	int Size() const {
		return m_MethodChain.Size();
	}

	template <typename TInvoker>
	TEvent& operator+=(const TInvoker& fn) {
		Register(fn);
		return *this;
	}

	template <typename TInvoker>
	TEvent& operator-=(const TInvoker& fn) {
		Unregister(fn);
		return *this;
	}

	void operator()(Args&&... args) {
		Invoke(Forward<Args>(args)...);
	}
private:
	LinkedList<Callback> m_MethodChain;
};



NS_JC_END