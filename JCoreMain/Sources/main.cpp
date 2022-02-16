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
{
	A(){}
	A(A&&) { cout << "ÀÌµ¿»ý¼ºÀÚ\n"; }
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

template <typename Fn>
bool Tefasefesast(const Model& m, Fn fn) {
	return fn(m);
}

int main() {
	cout << Tefasefesast({ 0 }, [](const Model& m) {return m.a == 0; }) << "\n";
	cout << Tefasefesast({ 0 }, FT()) << "\n";

	unordered_map<int, int> gggg;
	int b = 4;
	std::pair<int, int> fdsfdscsd = make_pair(1, b);
	gggg.insert(make_pair(1, 5));



	for (int i = 0; i < 50; i++) {
		map.Insert(MakePair(i, i));
	}

	map.PrintInfo();

	SharedPointer<HashMapIterator<int, int>> it = map.Begin();

	cout << "=================\n";
	int i = 0;
	while (it->HasNext()) {
		KeyValuePair<int, int> val = it->Next();
		printf("\tÅ° : %d, °ª : %d\n", val.Key, val.Value);
		i++;
	}
	cout << i << "\n";

	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		

