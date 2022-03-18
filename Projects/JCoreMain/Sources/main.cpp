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


int main() {
	MemoryLeakDetector detector;

	int k = 0xaabbccdd;
	int* z = &k;

	CriticalSectionMutex mutex;
	mutex.Lock();

	mutex.Unlock();

	return 0;
}


		

		
		



