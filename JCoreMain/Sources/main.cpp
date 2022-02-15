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


int main() {
	{
		MemoryLeakDetector detector;
		LinkedList<Model> m;
		m.PushBack({ 1 });
		m.PushBack({ 2 });
		m.PushBack({ 3 });
		m.PushBack({ 4 });
	}
	


	Vector<int> vec;
	vec.PushBack(1);
	vec.PushBack(2);
	vec.PushBack(3);
	vec.PushBack(4);
	vec.PushBack(5);
	vec.PushBack(6);

	vec.Insert(0, 1);
	vec.Insert(0, 2);
	vec.Insert(0, 3);

	for (int i = 0; i < vec.Size(); i++) {
		cout << vec[i] << "\n";
	}

	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		

