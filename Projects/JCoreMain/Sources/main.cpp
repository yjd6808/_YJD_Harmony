#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


struct Model
{
	Model() { }
	Model(int da) { a = da; cout << StringUtil::Format("�� %dȣ ����\n", da); }
	virtual ~Model() { cout << StringUtil::Format("�� %dȣ �Ҹ�\n", a); }

	int a = 3;
	int b = 3;
};

struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { cout << ("���� �� �Ҹ�\n"); }
};


template <typename T>
struct ModelTemplate
{
	ModelTemplate(int da) { a = da; cout << ("�� 1ȣ ����\n"); }
	virtual ~ModelTemplate() { cout << ("�� 1ȣ �Ҹ�\n"); }

	T a = 3;
	T b = 3;
};

template <typename T>
struct SuperModelTempalte : ModelTemplate<T>
{
	SuperModelTempalte() : ModelTemplate<T>(1) {}
	~SuperModelTempalte() override { cout << ("���� �� �Ҹ�\n"); }
};


class Test
{
public:
	Test() : owner(this, true) {}
	virtual ~Test() { owner.~VoidOwner(); }

	
	union { VoidOwner owner; };
protected:
	virtual void A() {
		cout << "a\n";
	}
protected:
	int a;
};


class Derived : public Test
{
public:
	Derived() : Test() {}
	virtual ~Derived() {  }
public:
	void A() override {
		Test::A();
		cout << "derived\n";
	}
};

class Legendary : public Derived
{
	void A() override {
	}
};


struct Zizon
{
	Zizon(){}
	~Zizon() {}
	union { Model model; };
};


struct A
{/*
	A() { cout << "A ����Ʈ ������\n"; }
	A(int a) : a(a) { cout << "A �Ϲ� ������\n"; }
	A(const A&) { cout << "A ���� ������\n"; }
	A(A&&) { cout << "A �̵� ������\n"; }
	~A() { cout << "A �Ҹ���\n"; }

	A& operator=(const A&) = default;
		A& operator=(A&&) = default;
*/
	int a;
};

struct B
{
	A le;
	A le2;
};
#include <JCore/Container/HashMap.h>

HashMap<int, int> map;

struct FT
{
	bool operator() (const Model& m) {
		return m.a == 0;
	}
};

struct FT2
{
	void operator() (int& m) {
		cout << m << "\n";
	}
};

template <typename Fn>
bool Tefasefesast(const Model& m, Fn fn) {
	return fn(m);
}

void fasfesaf(int& m, std::function<void(int&)> fn) {
	fn(m);
}


/* ���� ����
	struct ICallback
	{
		int*	FnPointer;
		TAction Action;

		virtual void	  		 Invoke(Args&&...) = 0;
		virtual const type_info& TargetType() = 0;
	};
	*/


template <typename... Args>
class Event
{
private:
	using TEvent  = typename Event<Args...>;
	using TAction = typename Action<Args...>;
private:
	
	struct Callback
	{
		void*	FnPointer;
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
			return fnType == call->TargetType();
		});
	}

	void Clear() {
		m_MethodChain.Clear();
	}

	void Invoke(Args&&... params) {
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






void invoke1_(int a, int b) {
	cout << "invoke1_  " << a << " " << b << "\n";
}

auto invoke2_ = [](int a, int b) {
	cout << "invoke2_  " << a << " " << b << "\n";
};

struct Invoker3 {
	void operator()(int a, int b) {
		cout << "invoke3_  " << a << " " << b << "\n";
	}
};

int main() {
	Event<int, int> e;
	Invoker3 invoke3_;

	// �� Ÿ�Ժ� �ֱ�
	e += invoke1_;
	e += invoke2_;
	e += invoke3_;
	e(1, 2);
	cout << "=================\n";

	// �ڵ鷯 ����
	e -= invoke3_;
	e(1, 2);
	cout << "=================\n";


	// ��� �ڵ鷯 �߰��ߴµ� �� ���� ����
	{
		e += [](int a, int b) {
			cout << "invoke4_  " << a << " " << b << "\n";
		};
	}
	e(1, 5);
	return 0;
}