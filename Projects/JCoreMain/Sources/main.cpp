#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


struct Model
{
	Model() { }
	Model(int da) { a = da; cout << StringUtil::Format("¸ðµ¨ %dÈ£ »ý¼º\n", da); }
	virtual ~Model() { cout << StringUtil::Format("¸ðµ¨ %dÈ£ ¼Ò¸ê\n", a); }

	int a = 3;
	int b = 3;
};

struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { cout << ("½´ÆÛ ¸ðµ¨ ¼Ò¸ê\n"); }
};


template <typename T>
struct ModelTemplate
{
	ModelTemplate(int da) { a = da; cout << ("¸ðµ¨ 1È£ »ý¼º\n"); }
	virtual ~ModelTemplate() { cout << ("¸ðµ¨ 1È£ ¼Ò¸ê\n"); }

	T a = 3;
	T b = 3;
};

template <typename T>
struct SuperModelTempalte : ModelTemplate<T>
{
	SuperModelTempalte() : ModelTemplate<T>(1) {}
	~SuperModelTempalte() override { cout << ("½´ÆÛ ¸ðµ¨ ¼Ò¸ê\n"); }
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
	A() { cout << "A µðÆúÆ® »ý¼ºÀÚ\n"; }
	A(int a) : a(a) { cout << "A ÀÏ¹Ý »ý¼ºÀÚ\n"; }
	A(const A&) { cout << "A º¹»ç »ý¼ºÀÚ\n"; }
	A(A&&) { cout << "A ÀÌµ¿ »ý¼ºÀÚ\n"; }
	~A() { cout << "A ¼Ò¸êÀÚ\n"; }

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


/* ¾µ¸ð ¾øÀ½
	struct ICallback
	{
		int*	FnPointer;
		TAction Action;

		virtual void	  		 Invoke(Args&&...) = 0;
		virtual const type_info& TargetType() = 0;
	};
	*/







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
	
	Vector<Model*> g;



	return 0;
}