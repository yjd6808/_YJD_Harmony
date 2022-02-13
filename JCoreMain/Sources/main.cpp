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


int main() {

	VoidWatcher w;
	if (w == nullptr) {
		int a = 40;
	}

	Derived d;
	d.A();

	std::vector<Model> m;
	{
		ArrayStack<SuperModel> stack;
		stack.EmplacePush();
		stack.EmplacePush();
		SharedPointer<Iterator<SuperModel>> iter = stack.Begin();

		while (iter->HasValue()) {
			cout << iter->Next().a << "\n";
		}
	}

	{
		queue<Model> q2;
		ArrayQueue<Model> queue;

		for (int i = 0; i < 20; i++) {
			queue.Enqueue({ i });
		}

		Enumerator<Model> it = queue.Begin();
		while (it->HasValue()) {
			cout << "����� : " << it->Next().a << "\n";
		}

		queue.Clear();
		/*
		for (int i = 0; i < 20; i++) {
			queue.Dequeue();
		}
		*/



		for (int i = 0; i < 40; i++) {
			queue.Enqueue({ i });
		}

		it = queue.Begin();
		while (it->HasValue()) {
			cout << "����� : " << it->Next().a << "\n";
		}

		for (int i = 0; i < 20; i++) {
			queue.Dequeue();
		}

		for (int i = 0; i < 40; i++) {
			queue.Enqueue({ i });
		}

		it = queue.Begin();
		while (it->HasValue()) {
			cout << "����� : " << it->Next().a << "\n";
		}
	}
	
	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		

